/**
 * @file    w9825g6kh.c
 * @author  Jia Zhenyu
 * @brief   W9825G6KH SDRAM 驱动实现文件
 *
 * @details 本文件实现了 W9825G6KH SDRAM 的初始化、8/16/32 位数据的读写操作，
 *          支持 HAL 方式、直接访问和 memcpy 方式，并包含 DMA 读写接口。
 *          适用于基于 STM32 FMC 控制器的 SDRAM 驱动场景。
 *
 * @version 1.0.0
 * @date    2024-07-30
 */

#include "w9825g6kh.h"
#include <string.h>

// SDRAM 命令发送封装
static HAL_StatusTypeDef sdram_send_cmd(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *cmd)
{
    return HAL_SDRAM_SendCommand(hsdram, cmd, HAL_MAX_DELAY);
}

/**
 * @brief 初始化 W9825G6KH 型号的 SDRAM
 *
 * 本函数负责初始化 W9825G6KH 型号的 SDRAM，包括配置 base_addr、target_bank 以及执行必要的 SDRAM 命令
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param hsdram SDRAM 的句柄指针
 * @param base_addr SDRAM 的基础地址 (W9825G6KH_BASEADDR1/W9825G6KH_BASEADDR2)
 * @param target_bank SDRAM 的目标银行 (FMC_SDRAM_CMD_TARGET_BANK1/FMC_SDRAM_CMD_TARGET_BANK2/FMC_SDRAM_CMD_TARGET_BANK1_2)
 * @return int 返回 0 表示成功，负数表示错误代码
 */
int w9825g6kh_init(W9825G6KH *hw9825g6kh, SDRAM_HandleTypeDef *hsdram, uint32_t base_addr, uint32_t target_bank)
{
    // 参数合法性检查
    if (!hw9825g6kh || !hsdram)
    {
        return -1;
    }
    if ((base_addr != W9825G6KH_BASEADDR1) && (base_addr != W9825G6KH_BASEADDR2))
    {
        return -2;
    }
    if ((target_bank != FMC_SDRAM_CMD_TARGET_BANK1) && (target_bank != FMC_SDRAM_CMD_TARGET_BANK2) && (target_bank != FMC_SDRAM_CMD_TARGET_BANK1_2))
    {
        return -3;
    }

    // hw9825g6kh 初始化
    hw9825g6kh->hsdram = hsdram;
    hw9825g6kh->base_addr = base_addr;
    hw9825g6kh->size_bytes = W9825G6KH_SIZEBYTES;
    hw9825g6kh->target_bank = target_bank;

    FMC_SDRAM_CommandTypeDef Command;

    // 1. 配置时钟启用命令
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = hw9825g6kh->target_bank;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    if (sdram_send_cmd(hw9825g6kh->hsdram, &Command) != HAL_OK)
    {
        return -4;
    }

    HAL_Delay(1); // 延时至少 100us

    // 2. 预充电命令，针对所有 bank
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = hw9825g6kh->target_bank;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    if (sdram_send_cmd(hw9825g6kh->hsdram, &Command) != HAL_OK)
    {
        return -5;
    }

    // 3. 自动刷新命令，执行 8 次刷新
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = hw9825g6kh->target_bank;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;
    if (sdram_send_cmd(hw9825g6kh->hsdram, &Command) != HAL_OK)
    {
        return -6;
    }

    // 4. 配置模式寄存器
    uint32_t mode_reg = SDRAM_MODEREG_BURST_LENGTH_1 |
                        SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
                        SDRAM_MODEREG_CAS_LATENCY_3 |
                        SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                        SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = hw9825g6kh->target_bank;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = mode_reg;
    if (sdram_send_cmd(hw9825g6kh->hsdram, &Command) != HAL_OK)
    {
        return -7;
    }

    // 5. 设置刷新计数器（STM32F429 最大 90MHz 时钟，刷新周期 7.81us）
    /*
        计算公式：Count = Refresh_rate * SDRAM_Clock / 1000 - 20（留余量）
        对于 64ms / 8192 行，约为 64ms/8192 = 7.81us 刷新周期
        7.81us * 90MHz = 703，常用 683 或 683~743 都行
    */
    uint32_t refresh_count = 683;
    if (HAL_SDRAM_ProgramRefreshRate(hw9825g6kh->hsdram, refresh_count) != HAL_OK)
    {
        return -8;
    }

    return 0;
}

// 读写时计算对应地址
static inline volatile void *w9825g6kh_addr(W9825G6KH *hw9825g6kh, uint32_t address)
{
    return (volatile void *)(hw9825g6kh->base_addr + address);
}

/**
 * @brief 从 W9825G6KH 设备中阻塞读取 8 位数据
 *
 * 此函数用于从指定地址开始，读取一定长度的 8 位数据到提供的缓冲区中
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要读取数据的起始地址
 * @param buf 存储读取数据的缓冲区指针，不能为空
 * @param len 要读取的数据长度，以 uint8_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_read8_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }

    volatile uint8_t *addr = (volatile uint8_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Read_8b(hw9825g6kh->hsdram, (uint32_t *)addr, buf, len) == HAL_OK) ? 0 : -3;
}

int w9825g6kh_read8(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2;
    }

    volatile uint8_t *src = (volatile uint8_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        buf[i] = src[i]; // 从 SDRAM 读入 buf
    }
    return 0;
}

int w9825g6kh_read8_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2;
    }

    void *src = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(buf, src, len);
    return 0;
}

/**
 * @brief 向 W9825G6KH 设备中阻塞写入 8 位数据
 *
 * 该函数用于将一定长度的 8 位数据写入到 W9825G6KH 设备的指定地址
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要写入数据的起始地址
 * @param buf 指向要写入数据的缓冲区
 * @param len 要写入数据的长度，以 uint8_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_write8_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }

    volatile uint8_t *addr = (volatile uint8_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Write_8b(hw9825g6kh->hsdram, (uint32_t *)addr, (uint8_t *)buf, len) == HAL_OK) ? 0 : -3;
}

int w9825g6kh_write8(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2;
    }

    volatile uint8_t *dst = (volatile uint8_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        dst[i] = buf[i]; // 写入 SDRAM
    }
    return 0;
}

int w9825g6kh_write8_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len) > hw9825g6kh->size_bytes)
    {
        return -2;
    }

    void *dst = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(dst, buf, len);
    return 0;
}

/**
 * 从 W9825G6KH 设备中阻塞读取 16 位数据
 *
 * 此函数用于从指定地址开始，读取指定长度的 16 位数据到缓冲区中
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要读取数据的起始地址
 * @param buf 存储读取数据的缓冲区指针
 * @param len 要读取的数据长度，以 uint16_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_read16_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    volatile uint16_t *addr = (volatile uint16_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Read_16b(hw9825g6kh->hsdram, (uint32_t *)addr, buf, len) == HAL_OK) ? 0 : -4;
}

int w9825g6kh_read16(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    volatile uint16_t *src = (volatile uint16_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        buf[i] = src[i];
    }
    return 0;
}

int w9825g6kh_read16_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    void *src = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(buf, src, len * sizeof(uint16_t));
    return 0;
}

/**
 * @brief 向 W9825G6KH 设备中阻塞写入 16 位数据
 *
 * 该函数用于将一定长度的 16 位数据写入到 W9825G6KH 设备的指定地址
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要写入数据的起始地址
 * @param buf 指向要写入数据的缓冲区
 * @param len 要写入数据的长度，以 uint16_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_write16_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    volatile uint16_t *addr = (volatile uint16_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Write_16b(hw9825g6kh->hsdram, (uint32_t *)addr, (uint16_t *)buf, len) == HAL_OK) ? 0 : -4;
}

int w9825g6kh_write16(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    volatile uint16_t *dst = (volatile uint16_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        dst[i] = buf[i];
    }
    return 0;
}

int w9825g6kh_write16_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 2) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 2 != 0)
    {
        return -3;
    }

    void *dst = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(dst, buf, len * sizeof(uint16_t));
    return 0;
}

/**
 * 从 W9825G6KH 设备中阻塞读取 32 位数据
 *
 * 此函数用于从指定地址开始，读取指定长度的 32 位数据到缓冲区中
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要读取数据的起始地址
 * @param buf 存储读取数据的缓冲区指针
 * @param len 要读取的数据长度，以 uint32_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_read32_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *addr = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Read_32b(hw9825g6kh->hsdram, (uint32_t *)addr, buf, len) == HAL_OK) ? 0 : -4;
}

int w9825g6kh_read32(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *src = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        buf[i] = src[i];
    }
    return 0;
}

int w9825g6kh_read32_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    void *src = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(buf, src, len * sizeof(uint32_t));
    return 0;
}

/**
 * @brief 向 W9825G6KH 设备中阻塞写入 32 位数据
 *
 * 该函数用于将一定长度的 32 位数据写入到 W9825G6KH 设备的指定地址
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要写入数据的起始地址
 * @param buf 指向要写入数据的缓冲区
 * @param len 要写入数据的长度，以 uint32_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_write32_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *addr = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Write_32b(hw9825g6kh->hsdram, (uint32_t *)addr, (uint32_t *)buf, len) == HAL_OK) ? 0 : -4;
}

int w9825g6kh_write32(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *dst = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    for (uint32_t i = 0; i < len; ++i)
    {
        dst[i] = buf[i];
    }
    return 0;
}

int w9825g6kh_write32_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    void *dst = (void *)w9825g6kh_addr(hw9825g6kh, address);
    memcpy(dst, buf, len * sizeof(uint32_t));
    return 0;
}

/**
 * 从 W9825G6KH 设备中 DAM 方式（非阻塞）读取 32 位数据
 *
 * 此函数用于从指定地址开始，读取指定长度的 32 位数据到缓冲区中
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要读取数据的起始地址
 * @param buf 存储读取数据的缓冲区指针
 * @param len 要读取的数据长度，以 uint32_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_read_dma_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *addr = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Read_DMA(hw9825g6kh->hsdram, (uint32_t *)addr, buf, len) == HAL_OK) ? 0 : -4;
}

/**
 * @brief 向 W9825G6KH 设备中 DAM 方式（非阻塞）写入 32 位数据
 *
 * 该函数用于将一定长度的 32 位数据写入到 W9825G6KH 设备的指定地址
 *
 * @param hw9825g6kh W9825G6KH 设备句柄
 * @param address 要写入数据的起始地址
 * @param buf 指向要写入数据的缓冲区
 * @param len 要写入数据的长度，以 uint32_t 为单位
 * @return int 返回 0 表示成功，负值表示错误代码
 */
int w9825g6kh_write_dma_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len)
{
    if (!hw9825g6kh || !buf)
    {
        return -1;
    }
    if ((address + len * 4) > hw9825g6kh->size_bytes)
    {
        return -2; // 越界检测
    }
    if (address % 4 != 0)
    {
        return -3;
    }

    volatile uint32_t *addr = (volatile uint32_t *)w9825g6kh_addr(hw9825g6kh, address);
    return (HAL_SDRAM_Write_DMA(hw9825g6kh->hsdram, (uint32_t *)addr, (uint32_t *)buf, len) == HAL_OK) ? 0 : -4;
}
