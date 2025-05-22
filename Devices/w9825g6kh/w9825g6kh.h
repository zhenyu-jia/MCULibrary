/**
 * @file    w9825g6kh.h
 * @author  Jia Zhenyu
 * @brief   W9825G6KH SDRAM 驱动头文件
 *
 * @details 提供 W9825G6KH SDRAM 芯片在 STM32 平台下的初始化、8/16/32 位数据读写、DMA 操作等接口声明。
 *          支持 HAL、直接指针和 memcpy 三种访问方式，适用于基于 STM32 FMC 控制器的应用场景。
 * @version 1.0.0
 * @date    2024-07-30
 */

#ifndef __W9825G6KH_H__
#define __W9825G6KH_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h" // 根据你使用的 STM32 系列修改头文件

/**
 * @brief FMC SDRAM 模式配置的寄存器相关定义
 */
// 突发长度设置
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
// 连续还是间隔模式
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
// CL 等待几个时钟周期
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
// 正常模式
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
// 设置写操作的模式，可以选择突发模式或者单次写入模式
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

#define W9825G6KH_BASEADDR1 0xC0000000
#define W9825G6KH_BASEADDR2 0xD0000000
#define W9825G6KH_SIZEBYTES (4 * 1024 * 1024 * 4 * 2) // 4M words * 4 banks * 16 bits

    typedef struct
    {
        SDRAM_HandleTypeDef *hsdram; // HAL SDRAM 句柄
        uint32_t base_addr;          // SDRAM 映射起始地址
        uint32_t size_bytes;         // 容量，单位字节
        uint32_t target_bank;        // FMC_SDRAM_CMD_TARGET_BANKx
    } W9825G6KH;

    // 初始化
    int w9825g6kh_init(W9825G6KH *hw9825g6kh, SDRAM_HandleTypeDef *hsdram, uint32_t base_addr, uint32_t target_bank);

    // 读写接口
    // 使用 HAL 库访问
    int w9825g6kh_read8_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len);
    int w9825g6kh_write8_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len);
    int w9825g6kh_read16_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len);
    int w9825g6kh_write16_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len);
    int w9825g6kh_read32_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len);
    int w9825g6kh_write32_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len);
    int w9825g6kh_read_dma_hal(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len);
    int w9825g6kh_write_dma_hal(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len);

    // 通过指针直接访问
    int w9825g6kh_read8(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len);
    int w9825g6kh_write8(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len);
    int w9825g6kh_read16(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len);
    int w9825g6kh_write16(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len);
    int w9825g6kh_read32(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len);
    int w9825g6kh_write32(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len);

    // memcpy 方式
    int w9825g6kh_read8_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint8_t *buf, uint32_t len);
    int w9825g6kh_write8_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint8_t *buf, uint32_t len);
    int w9825g6kh_read16_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint16_t *buf, uint32_t len);
    int w9825g6kh_write16_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint16_t *buf, uint32_t len);
    int w9825g6kh_read32_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, uint32_t *buf, uint32_t len);
    int w9825g6kh_write32_memcpy(W9825G6KH *hw9825g6kh, uint32_t address, const uint32_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __W9825G6KH_H__ */
