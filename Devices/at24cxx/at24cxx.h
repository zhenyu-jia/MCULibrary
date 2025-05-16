/**
 * @file at24cxx.h
 * @brief AT24CXX EEPROM 驱动库头文件
 *
 * 该头文件定义了 AT24CXX 系列 EEPROM 的初始化、就绪检查、写入、读取等操作的接口。
 * 支持常见型号如 AT24C01, AT24C02, AT24C04, AT24C08, AT24C16,
 *                AT24C32, AT24C64, AT24C128, AT24C256, AT24C512等。
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 */

#ifndef __AT24CXX_H__
#define __AT24CXX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h" // 根据你使用的 STM32 系列修改头文件

/** @defgroup AT24CXX_Constants AT24CXX 常量定义
 * @{
 */
#define WRITE_CYCLE_TIME 5 /**< EEPROM 写入周期（单位：毫秒） */

/** @defgroup AT24CXX_Sizes AT24CXX 各型号 EEPROM 存储大小（单位：字节）
 * @{
 */
#define AT24C01_SIZE 128    /**< AT24C01 EEPROM 大小 */
#define AT24C02_SIZE 256    /**< AT24C02 EEPROM 大小 */
#define AT24C04_SIZE 512    /**< AT24C04 EEPROM 大小 */
#define AT24C08_SIZE 1024   /**< AT24C08 EEPROM 大小 */
#define AT24C16_SIZE 2048   /**< AT24C16 EEPROM 大小 */
#define AT24C32_SIZE 4096   /**< AT24C32 EEPROM 大小 */
#define AT24C64_SIZE 8192   /**< AT24C64 EEPROM 大小 */
#define AT24C128_SIZE 16384 /**< AT24C128 EEPROM 大小 */
#define AT24C256_SIZE 32768 /**< AT24C256 EEPROM 大小 */
#define AT24C512_SIZE 65536 /**< AT24C512 EEPROM 大小 */
/** @} */

/** @defgroup AT24CXX_PageSizes AT24CXX 各型号 EEPROM 页大小（单位：字节）
 * @{
 */
#define AT24C01_PAGE_SIZE 8    /**< AT24C01 EEPROM 页大小 */
#define AT24C02_PAGE_SIZE 8    /**< AT24C02 EEPROM 页大小 */
#define AT24C04_PAGE_SIZE 16   /**< AT24C04 EEPROM 页大小 */
#define AT24C08_PAGE_SIZE 16   /**< AT24C08 EEPROM 页大小 */
#define AT24C16_PAGE_SIZE 16   /**< AT24C16 EEPROM 页大小 */
#define AT24C32_PAGE_SIZE 32   /**< AT24C32 EEPROM 页大小 */
#define AT24C64_PAGE_SIZE 32   /**< AT24C64 EEPROM 页大小 */
#define AT24C128_PAGE_SIZE 64  /**< AT24C128 EEPROM 页大小 */
#define AT24C256_PAGE_SIZE 64  /**< AT24C256 EEPROM 页大小 */
#define AT24C512_PAGE_SIZE 128 /**< AT24C512 EEPROM 页大小 */
    /** @} */

    /** @defgroup AT24CXX_Types AT24CXX EEPROM 类型定义
     * @{
     */
    typedef enum
    {
        AT24C01,  /**< AT24C01 型号 EEPROM */
        AT24C02,  /**< AT24C02 型号 EEPROM */
        AT24C04,  /**< AT24C04 型号 EEPROM */
        AT24C08,  /**< AT24C08 型号 EEPROM */
        AT24C16,  /**< AT24C16 型号 EEPROM */
        AT24C32,  /**< AT24C32 型号 EEPROM */
        AT24C64,  /**< AT24C64 型号 EEPROM */
        AT24C128, /**< AT24C128 型号 EEPROM */
        AT24C256, /**< AT24C256 型号 EEPROM */
        AT24C512  /**< AT24C512 型号 EEPROM */
    } AT24CXX_Type;
    /** @} */

    /** @defgroup AT24CXX_Handle AT24CXX 设备结构体定义
     * @{
     */
    typedef struct
    {
        AT24CXX_Type memory_type; /**< EEPROM 类型 */
        I2C_HandleTypeDef *hi2c;  /**< I2C 外设句柄 */
        uint8_t device_address;   /**< EEPROM 设备地址 */
        uint32_t memory_size;     /**< EEPROM 总存储大小 */
        uint16_t page_size;       /**< EEPROM 页大小 */
    } AT24CXX;
    /** @} */
    /** @} */

    /**
     * @brief 初始化 AT24CXX EEPROM
     *
     * @param hat24cxx 指向 AT24CXX 设备句柄的指针
     * @param memory_type EEPROM 类型（AT24C01, AT24C02, ...）
     * @param hi2c 指向 I2C 外设句柄的指针
     * @param device_address EEPROM 设备地址（0~7）
     *
     * @return 0 如果初始化成功，其他负值表示错误
     */
    int at24cxx_init(AT24CXX *hat24cxx, AT24CXX_Type memory_type, I2C_HandleTypeDef *hi2c, uint8_t device_address);

    /**
     * @brief 检查 AT24CXX 设备是否就绪
     *
     * @param hat24cxx 指向 AT24CXX 设备句柄的指针
     * @param trials 尝试次数
     * @param timeout 超时时间（单位：毫秒）
     *
     * @return 0 如果设备就绪，其他负值表示错误
     */
    int at24cxx_ready(AT24CXX *hat24cxx, uint32_t trials, uint32_t timeout);

    /**
     * @brief 向 AT24CXX EEPROM 写入数据
     *
     * @param hat24cxx 指向 AT24CXX 设备句柄的指针
     * @param mem_address 要写入的内存地址
     * @param pData 要写入的数据指针
     * @param size 要写入的数据大小
     *
     * @return 0 如果写入成功，其他负值表示错误
     */
    int at24cxx_write(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size);

    /**
     * @brief 从 AT24CXX EEPROM 读取数据
     *
     * @param hat24cxx 指向 AT24CXX 设备句柄的指针
     * @param mem_address 要读取的内存地址
     * @param pData 存放读取数据的指针
     * @param size 要读取的数据大小
     *
     * @return 0 如果读取成功，其他负值表示错误
     */
    int at24cxx_read(AT24CXX *hat24cxx, uint32_t mem_address, uint8_t *pData, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* __AT24CXX_H__ */
