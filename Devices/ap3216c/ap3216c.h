/**
 * @file    ap3216c.h
 * @author  Jia Zhenyu
 * @version 1.0.0
 * @date    2024-08-02
 * @brief   AP3216C 传感器驱动头文件
 *
 * 本头文件声明了 AP3216C 红外、光照、接近传感器的相关数据结构、宏定义及操作函数。
 * 适用于基于 STM32 HAL 库的 I2C 通信。
 *
 * 使用方法：
 *  1. 包含本头文件并配置好 I2C 外设。
 *  2. 调用 ap3216c_init() 初始化设备。
 *  3. 使用 ap3216c_get_ir_data()、ap3216c_get_als_data()、ap3216c_get_ps_data() 等函数获取传感器数据。
 *
 * @note    需包含 stm32f4xx_hal.h 头文件或根据实际 MCU 修改。
 */

#ifndef __AP3216C_H__
#define __AP3216C_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f4xx_hal.h" // 根据你使用的 STM32 系列修改头文件

#define AP3216C_ADDRESS 0X1E // AP3216C 器件地址

/* AP3216C 相关时间 */
#define AP3216C_STABLE_TIME 50                                                                                                   // 电源稳定时间（大于 10ms）
#define AP3216C_ALS_CONVERSION_TIME 100                                                                                          // ALS 转换时间（100ms）
#define AP3216C_PS_IR_CONVERSION_TIME 13                                                                                         // PS + IR 转换时间（12.5ms）
#define AP3216C_ALS_PS_IR_CONVERSION_TIME (uint16_t)(AP3216C_ALS_CONVERSION_TIME + AP3216C_PS_IR_CONVERSION_TIME)                // ALS + PS + IR 转换时间
#define AP3216C_ALS_ONCE_CONVERSION_TIME (uint16_t)(AP3216C_ALS_CONVERSION_TIME * 2.5)                                           // ALS 单次模式转换时间（2.5 倍）
#define AP3216C_PS_IR_ONCE_CONVERSION_TIME (uint16_t)(AP3216C_PS_IR_CONVERSION_TIME * 2.5)                                       // PS + IR 单次模式转换时间（2.5 倍）
#define AP3216C_ALS_PS_IR_ONCE_CONVERSION_TIME (uint16_t)(AP3216C_PS_IR_CONVERSION_TIME * 2.5 + AP3216C_ALS_CONVERSION_TIME * 2) // ALS + PS + IR 单次模式转换时间

/* AP3216C 寄存器 */
#define AP3216C_SYSTEM_CONFIG_ADDRESS 0x00 // 系统配置寄存器
#define AP3216C_INT_STATUS_ADDRESS 0X01    // 中断状态寄存器
#define AP3216C_INT_CLEAR_ADDRESS 0X02     // 中断清除寄存器
#define AP3216C_IR_DATA_LOW_ADDRESS 0x0A   // IR 数据低字节
#define AP3216C_IR_DATA_HIGH_ADDRESS 0x0B  // IR 数据高字节
#define AP3216C_ALS_DATA_LOW_ADDRESS 0x0C  // ALS 数据低字节
#define AP3216C_ALS_DATA_HIGH_ADDRESS 0X0D // ALS 数据高字节
#define AP3216C_PS_DATA_LOW_ADDRESS 0X0E   // PS 数据低字节
#define AP3216C_PS_DATA_HIGH_ADDRESS 0X0F  // PS 数据高字节

    typedef enum
    {
        POWER_DOWN = 0U,      // 掉电模式（默认）
        ALS,                  // ALS 功能激活
        PS_IR,                // PS + IR 功能激活
        ALS_PS_IR,            // ALS + PS + IR 功能激活
        SOFTWARE_RESET,       // 软复位
        ALS_ONCE,             // ALS 单次模式
        PS_IR_ONCE,           // PS + IR 单次模式
        ALS_PS_IR_ONCE        // ALS + PS + IR 单次模式
    } AP3216C_SystemModeType; /* AP3216C 系统模式功能 */

    typedef enum
    {
        IR_DATA,
        ALS_DATA,
        PS_DATA,
        PS_OBJECT_DETECT
    } AP3216C_DataType;

    typedef struct
    {
        I2C_HandleTypeDef *hi2c;
        uint8_t device_address;
        uint8_t system_mode;
        uint16_t ir_data;
        uint16_t als_data;
        uint16_t ps_data : 15;
        uint16_t ps_object_detect : 1;
    } AP3216C;

    int ap3216c_init(AP3216C *hap3216c, I2C_HandleTypeDef *hi2c, AP3216C_SystemModeType system_mode);
    int ap3216c_ready(AP3216C *hap3216c, uint32_t trials, uint32_t timeout);
    int ap3216c_set_system_mode(AP3216C *hap3216c, AP3216C_SystemModeType system_mode);
    int ap3216c_get_system_mode(AP3216C *hap3216c);
    int ap3216c_get_ir_data(AP3216C *hap3216c);
    int ap3216c_get_als_data(AP3216C *hap3216c);
    int ap3216c_get_ps_data(AP3216C *hap3216c);
    int ap3216c_get_ps_object_detect_data(AP3216C *hap3216c);

#ifdef __cplusplus
}
#endif

#endif /* __AP3216C_H__ */
