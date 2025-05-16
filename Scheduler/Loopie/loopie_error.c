/**
 * @file    loopie_error.c
 * @brief   错误与警告处理模块，实现错误码和警告码的设置、清除、报告等功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#include "loopie_error.h"
#include "loopie_config.h"
#include <stdint.h>
#include <stddef.h>

static report_func report_err_func = NULL;              // 错误报告函数指针
static report_func report_warn_func = NULL;             // 警告报告函数指针
static volatile uint32_t err_code_mask = NO_ERROR_MASK; // 错误码掩码
static volatile uint32_t warn_code = NO_WARNING;        // 警告码

/**
 * @brief  设置错误码
 *
 * @note   该函数将错误码掩码的第 err_count 位设置为 1，表示该错误发生
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval err_count
 */
int error_code_set(const uint8_t err_count)
{
    if (err_count > 31)
    {
        return -1;
    }
    err_code_mask |= (1U << err_count); // 将第 err_count 位设置为 1
    return err_count;
}

/**
 * @brief  清除错误码
 *
 * @note   该函数将错误码掩码的第 err_count 位清除为 0，表示该错误已被处理
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval err_count
 */
int error_code_clear(const uint8_t err_count)
{
    if (err_count > 31)
    {
        return -1;
    }
    err_code_mask &= ~(1U << err_count); // 清除第 err_count 位
    return err_count;
}

/**
 * @brief  清除所有错误码
 *
 * @param  None
 * @retval None
 */
void error_code_clear_all(void)
{
    err_code_mask = NO_ERROR_MASK;
}

/**
 * @brief  获取错误码
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval 当前的错误码
 */
uint8_t error_code_get(const uint8_t err_count)
{
    if (err_count > 31)
    {
        return 0; // 如果错误码索引无效，返回 0
    }
    return (err_code_mask & (1U << err_count)) ? 1 : 0; // 如果对应位为 1，表示错误发生
}

/**
 * @brief  获取所有错误码
 *
 * @param  None
 * @retval 当前的错误码掩码
 */
uint32_t error_code_get_all(void)
{
    return err_code_mask;
}

/**
 * @brief  设置警告码
 *
 * @param  warn  警告码
 * @retval warn
 */
uint32_t warning_code_set(const uint32_t warn)
{
    warn_code = warn;

    return warn;
}

/**
 * @brief  获取警告码
 *
 * @param  None
 * @retval 当前的警告码
 */
uint32_t warning_code_get(void)
{
    return warn_code;
}

/**
 * @brief  设置错误报告函数
 *
 * @param  func 错误报告函数的指针
 * @retval None
 */
void error_set_report_func(report_func func)
{
    if (!func)
    {
        return;
    }
    report_err_func = func;
}

/**
 * @brief  设置警告报告函数
 *
 * @param  func 警告报告函数的指针
 * @retval None
 */
void warning_set_report_func(report_func func)
{
    if (!func)
    {
        return;
    }
    report_warn_func = func;
}

/**
 * @brief  用来显示错误代码
 *
 * 可以用 LED、蜂鸣器、或者串口打印等
 *
 * @param  None
 * @retval None
 */
void error_report(void)
{
    static uint32_t pre_err_mask = NO_ERROR_MASK;

    // if (err_code_mask != pre_err_mask && err_code_mask != NO_ERROR_MASK)
    if (err_code_mask != pre_err_mask)
    {
        if (report_err_func != NULL)
        {
            report_err_func(err_code_mask);
        }
        pre_err_mask = err_code_mask;
    }
}

/**
 * @brief  用来显示警告代码
 *
 * 可以用 LED、蜂鸣器、或者串口打印等
 *
 * @param  None
 * @retval None
 */
void warning_report(void)
{
    static uint32_t pre_warn_code = NO_WARNING;
    static uint32_t warn_ticks_count = 0;

    if (warn_code != pre_warn_code)
    {
        if (report_warn_func != NULL)
        {
            report_warn_func(warn_code);
        }
        pre_warn_code = warn_code;

        if (warn_code != NO_WARNING)
        {
            warn_ticks_count = SCH_REPORT_WARNINGS_TICKS - 1; // 由于这行代码也会占用一个周期，所以这里要减 1

            if (warn_ticks_count == 0)
            {
                warn_code = NO_WARNING;
            }
        }
        else
        {
            warn_ticks_count = 0;
        }
    }
    else
    {
        if (warn_ticks_count != 0)
        {
            if (--warn_ticks_count == 0)
            {
                warn_code = NO_WARNING;
            }
        }
    }
}
