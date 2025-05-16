/**
 * @file at24cxx.c
 * @brief 实现 AT24CXX 系列 EEPROM 的初始化、就绪检查、写入、读取等操作的接口函数。
 *
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 */

#include "at24cxx.h"

/**
 * @brief 初始化 AT24CXX EEPROM
 *
 * 该函数初始化 AT24CXX EEPROM，设置 EEPROM 类型、I2C 设备地址及其他必要的参数。
 *
 * @param hat24cxx 指向 AT24CXX 设备句柄的指针
 * @param memory_type EEPROM 类型（AT24C01, AT24C02, ...）
 * @param hi2c 指向 I2C 外设句柄的指针
 * @param device_address EEPROM 设备地址（0~7）
 *
 * @return 0 如果初始化成功，其他负值表示错误：
 *         -1: 传入的参数为 NULL
 *         -2: EEPROM 型号不合法
 *         -3: 设备地址不合法
 *         -4: 页大小无效
 *         -5: 设备没有就绪
 */
int at24cxx_init(AT24CXX *hat24cxx, AT24CXX_Type memory_type, I2C_HandleTypeDef *hi2c, uint8_t device_address)
{
    if (hat24cxx == NULL || hi2c == NULL)
    {
        return -1;
    }

    // 检查设备型号是否合法
    if (memory_type > AT24C512)
    {
        return -2;
    }

    // 检查设备地址是否合法
    if (device_address > 0x07)
    {
        return -3;
    }

    hat24cxx->memory_type = memory_type;
    hat24cxx->hi2c = hi2c;
    hat24cxx->device_address = 0xA0 | ((device_address & 0x07) << 1);

    switch (hat24cxx->memory_type)
    {
    case AT24C01:
        hat24cxx->memory_size = AT24C01_SIZE;
        hat24cxx->page_size = AT24C01_PAGE_SIZE;
        break;
    case AT24C02:
        hat24cxx->memory_size = AT24C02_SIZE;
        hat24cxx->page_size = AT24C02_PAGE_SIZE;
        break;
    case AT24C04:
        hat24cxx->memory_size = AT24C04_SIZE;
        hat24cxx->page_size = AT24C04_PAGE_SIZE;
        break;
    case AT24C08:
        hat24cxx->memory_size = AT24C08_SIZE;
        hat24cxx->page_size = AT24C08_PAGE_SIZE;
        break;
    case AT24C16:
        hat24cxx->memory_size = AT24C16_SIZE;
        hat24cxx->page_size = AT24C16_PAGE_SIZE;
        break;
    case AT24C32:
        hat24cxx->memory_size = AT24C32_SIZE;
        hat24cxx->page_size = AT24C32_PAGE_SIZE;
        break;
    case AT24C64:
        hat24cxx->memory_size = AT24C64_SIZE;
        hat24cxx->page_size = AT24C64_PAGE_SIZE;
        break;
    case AT24C128:
        hat24cxx->memory_size = AT24C128_SIZE;
        hat24cxx->page_size = AT24C128_PAGE_SIZE;
        break;
    case AT24C256:
        hat24cxx->memory_size = AT24C256_SIZE;
        hat24cxx->page_size = AT24C256_PAGE_SIZE;
        break;
    case AT24C512:
        hat24cxx->memory_size = AT24C512_SIZE;
        hat24cxx->page_size = AT24C512_PAGE_SIZE;
        break;
    default:
        return -2; // Invalid memory type
    }

    // 检查页大小是否合法（大于 0 且为 2 的幂）
    if ((hat24cxx->page_size <= 0) || ((hat24cxx->page_size & (hat24cxx->page_size - 1)) != 0))
    {
        return -4;
    }

    // 设备是否就绪
    if (at24cxx_ready(hat24cxx, 3, 1000))
    {
        return -5;
    }

    return 0;
}

/**
 * @brief 检查 AT24CXX 设备是否就绪
 *
 * 该函数用于检查 AT24CXX 是否已准备好进行操作。
 *
 * @param hat24cxx 指向 AT24CXX 设备句柄的指针
 * @param trials 尝试次数
 * @param timeout 超时时间（单位：毫秒）
 *
 * @return 0 如果设备就绪，其他负值表示错误：
 *         -1: 传入的参数为 NULL
 *         -2: 设备未准备好
 */
int at24cxx_ready(AT24CXX *hat24cxx, uint32_t trials, uint32_t timeout)
{
    if (hat24cxx == NULL)
    {
        return -1;
    }

    if (HAL_I2C_IsDeviceReady(hat24cxx->hi2c, hat24cxx->device_address, trials, timeout) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

/**
 * @brief 向 AT24CXX EEPROM 写入数据
 *
 * 该函数将数据写入 AT24CXX EEPROM。支持跨页写入，确保数据在 EEPROM 中按正确的地址顺序写入。
 * 每次写入后会延时以等待写入周期完成。
 *
 * @param hat24cxx 指向 AT24CXX 设备句柄的指针
 * @param mem_address 要写入的内存地址
 * @param pData 要写入的数据指针
 * @param size 要写入的数据大小
 *
 * @return 0 如果写入成功，其他负值表示错误：
 *         -1: 传入的参数为 NULL
 *         -2: 数据大小不合法
 *         -3: 写入地址不合法
 *         -4: I2C 写入失败
 */
int at24cxx_write(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size)
{
    if (hat24cxx == NULL || pData == NULL)
    {
        return -1;
    }

    // 检查 size 是否合法
    if (size == 0 || (size > hat24cxx->memory_size))
    {
        return -2;
    }

    // 检查写入地址是否合法
    if (mem_address + size > hat24cxx->memory_size)
    {
        return -3;
    }

    uint16_t mem_address_size = (hat24cxx->memory_type <= AT24C02) ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT;
    uint16_t page_size = hat24cxx->page_size;

    // 计算页地址和页内偏移地址
    uint16_t page_address = mem_address / page_size;       // 页地址
    uint8_t page_offset_address = mem_address % page_size; // 页内偏移地址

    uint16_t remaining_size = size;
    while (remaining_size > 0)
    {
        // 计算每次写入的字节数
        uint16_t data_to_write = (remaining_size <= (page_size - page_offset_address)) ? remaining_size : (page_size - page_offset_address);

        // 写入数据
        if (HAL_I2C_Mem_Write(hat24cxx->hi2c, hat24cxx->device_address, mem_address, mem_address_size, pData, data_to_write, 1000) != HAL_OK)
        {
            return -4;
        }

        // 更新剩余数据和地址
        remaining_size -= data_to_write;
        pData += data_to_write;
        mem_address += data_to_write;
        page_address++;          // 跳到下一页
        page_offset_address = 0; // 每次新的一页偏移地址为 0

        // 此处的延时可以根据修改成 RTOS 中的或者用定时器轮询的方式，防止阻塞
        HAL_Delay(WRITE_CYCLE_TIME);
    }

    return 0;
}

/**
 * @brief 从 AT24CXX EEPROM 读取数据
 *
 * 该函数从 AT24CXX EEPROM 读取指定地址的数据。
 *
 * @param hat24cxx 指向 AT24CXX 设备句柄的指针
 * @param mem_address 要读取的内存地址
 * @param pData 存放读取数据的指针
 * @param size 要读取的数据大小
 *
 * @return 0 如果读取成功，其他负值表示错误：
 *         -1: 传入的参数为 NULL
 *         -2: 数据大小不合法
 *         -3: 读取地址不合法
 *         -4: I2C 读取失败
 */
int at24cxx_read(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size)
{
    if (hat24cxx == NULL || pData == NULL)
    {
        return -1;
    }

    // 检查 size 是否合法
    if (size == 0 || (size > hat24cxx->memory_size))
    {
        return -2;
    }

    // 检查读取地址是否合法
    if (mem_address + size > hat24cxx->memory_size)
    {
        return -3;
    }

    uint16_t mem_address_size = (hat24cxx->memory_type <= AT24C02) ? I2C_MEMADD_SIZE_8BIT : I2C_MEMADD_SIZE_16BIT;

    // 读取数据
    if (HAL_I2C_Mem_Read(hat24cxx->hi2c, hat24cxx->device_address, mem_address, mem_address_size, pData, size, 1000) != HAL_OK)
    {
        return -4;
    }

    return 0;
}
