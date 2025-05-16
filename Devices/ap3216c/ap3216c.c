/**
 * @file    ap3216c.c
 * @author  Jia Zhenyu
 * @version 1.0.0
 * @date    2024-08-02
 * @brief   AP3216C 传感器驱动实现文件
 *
 * 本文件实现了 AP3216C 红外、光照、接近传感器的初始化、数据读取等基本操作。
 * 适用于基于 STM32 HAL 库的 I2C 通信。
 *
 * 使用方法：
 *  1. 调用 ap3216c_init() 初始化设备。
 *  2. 使用 ap3216c_get_ir_data()、ap3216c_get_als_data()、ap3216c_get_ps_data() 等函数获取传感器数据。
 *
 * @note    需包含 ap3216c.h 头文件并配置好 I2C 外设。
 */

#include "ap3216c.h"

/**
 * @brief AP3216C 初始化
 * @param hap3216c AP3216C 句柄
 * @param hi2c i2c 句柄
 * @param system_mode 系统模式
 * @return 0 成功，负数 失败
 */
int ap3216c_init(AP3216C *hap3216c, I2C_HandleTypeDef *hi2c, AP3216C_SystemModeType system_mode)
{
    if (hap3216c == NULL || hi2c == NULL)
    {
        return -1;
    }

    if (system_mode > ALS_PS_IR_ONCE)
    {
        return -2;
    }

    // 初始化 ap3216c 句柄
    hap3216c->hi2c = hi2c;
    hap3216c->device_address = AP3216C_ADDRESS << 1;
    hap3216c->system_mode = 0;
    hap3216c->ir_data = 0;
    hap3216c->als_data = 0;
    hap3216c->ps_data = 0;
    hap3216c->ps_object_detect = 0;

    // 设备是否就绪
    if (ap3216c_ready(hap3216c, 3, 1000))
    {
        return -3;
    }

    // 软复位
    if (ap3216c_set_system_mode(hap3216c, SOFTWARE_RESET))
    {
        return -4;
    }
    HAL_Delay(AP3216C_STABLE_TIME);

    // 设置系统模式
    if (ap3216c_set_system_mode(hap3216c, system_mode))
    {
        return -5;
    }
    // 读一下系统模式
    if (ap3216c_get_system_mode(hap3216c))
    {
        return -6;
    }
    else
    {
        if (hap3216c->system_mode != system_mode)
        {
            // 如果读成功且与设置的一样，会将 hap3216c->system_mode 设置为 system_mode
            return -7;
        }
    }

    return 0;
}

/**
 * @brief AP3216C 就绪
 * @param hap3216c AP3216C 结构体指针
 * @param trials 尝试次数
 * @param timeout 超时时间
 * @return 0 成功 负数 失败
 */
int ap3216c_ready(AP3216C *hap3216c, uint32_t trials, uint32_t timeout)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    if (HAL_I2C_IsDeviceReady(hap3216c->hi2c, hap3216c->device_address, trials, timeout) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

/**
 * @brief 设置系统模式
 * @param hap3216c AP3216C 设备句柄
 * @param system_mode 系统模式
 * @return 0 成功 负数 失败
 */
int ap3216c_set_system_mode(AP3216C *hap3216c, AP3216C_SystemModeType system_mode)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    if (system_mode > ALS_PS_IR_ONCE)
    {
        return -2;
    }

    if (HAL_I2C_Mem_Write(hap3216c->hi2c, hap3216c->device_address, AP3216C_SYSTEM_CONFIG_ADDRESS, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&system_mode, 1, 1000) != HAL_OK)
    {
        return -3;
    }

    return 0;
}

/**
 * @brief  获取系统模式
 * @param  hap3216c: AP3216C 设备句柄
 * @return 0 成功 负数 失败
 */
int ap3216c_get_system_mode(AP3216C *hap3216c)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_SYSTEM_CONFIG_ADDRESS, I2C_MEMADD_SIZE_8BIT, &hap3216c->system_mode, 1, 1000) != HAL_OK)
    {
        return -2;
    }

    return 0;
}

/**
 * @brief 获取 IR 数据
 *
 * @param hap3216c AP3216C 设备句柄
 * @param system_mode 系统模式
 * @return 正数 IR 数据 负数 失败
 */
int ap3216c_get_ir_data(AP3216C *hap3216c)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    uint8_t buff[2];

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_IR_DATA_LOW_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff, 1, 1000) != HAL_OK)
    {
        return -2;
    }

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_IR_DATA_HIGH_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff + 1, 1, 1000) != HAL_OK)
    {
        return -3;
    }

    if (buff[0] & 0x80) //  IR 数据无效
    {
        hap3216c->ir_data = 0;
        return -4;
    }
    else
    {
        hap3216c->ir_data = ((uint16_t)buff[1] << 2) | (buff[0] & 0x03);
    }

    return hap3216c->ir_data;
}

/**
 * @brief 获取 ALS 数据
 *
 * @param hap3216c AP3216C 设备句柄
 * @return 正数 ALS 数据 负数 失败
 */
int ap3216c_get_als_data(AP3216C *hap3216c)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    uint8_t buff[2];

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_ALS_DATA_LOW_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff, 1, 1000) != HAL_OK)
    {
        return -2;
    }

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_ALS_DATA_HIGH_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff + 1, 1, 1000) != HAL_OK)
    {
        return -3;
    }

    hap3216c->als_data = ((uint16_t)buff[1] << 8) | buff[0];

    return hap3216c->als_data;
}

/**
 * @brief 获取 PS 数据
 *
 * @param hap3216c AP3216C 设备句柄
 * @return 正数 PS 数据 负数 失败
 */
int ap3216c_get_ps_data(AP3216C *hap3216c)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    uint8_t buff[2];

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_PS_DATA_LOW_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff, 1, 1000) != HAL_OK)
    {
        return -2;
    }

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_PS_DATA_HIGH_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff + 1, 1, 1000) != HAL_OK)
    {
        return -3;
    }

    if (buff[0] & 0x40) //  PS 数据无效
    {
        hap3216c->ps_data = 0;
        return -4;
    }
    else
    {
        hap3216c->ps_data = ((uint16_t)(buff[1] & 0x3F) << 4) | (buff[0] & 0x0F);
    }

    return hap3216c->ps_data;
}

/**
 * @brief 获取 PS OBJECT DETECT 数据
 *
 * @param hap3216c AP3216C 设备句柄
 * @return 0 远离 1 接近
 */
int ap3216c_get_ps_object_detect_data(AP3216C *hap3216c)
{
    if (hap3216c == NULL)
    {
        return -1;
    }

    uint8_t buff[2];

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_PS_DATA_LOW_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff, 1, 1000) != HAL_OK)
    {
        return -2;
    }

    if (HAL_I2C_Mem_Read(hap3216c->hi2c, hap3216c->device_address, AP3216C_PS_DATA_HIGH_ADDRESS, I2C_MEMADD_SIZE_8BIT, buff + 1, 1, 1000) != HAL_OK)
    {
        return -3;
    }

    if (buff[0] & 0x40) //  PS 数据无效
    {
        hap3216c->ps_object_detect = 0;
        return -4;
    }
    else
    {
        hap3216c->ps_object_detect = (buff[0] & 0x80) >> 7;
    }

    return hap3216c->ps_object_detect;
}
