/**
 * @file    loopie_error.h
 * @brief   错误与警告处理模块头文件，声明错误码和警告码相关接口
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_ERROR_H__
#define __LOOPIE_ERROR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* 错误码，用掩码的方式定义，同时能够处理 32 个错误码 */
#define NO_ERROR_MASK 0U

/* 警告码，是数值，不是掩码 */
#define NO_WARNING 0U

    /**
     * @brief  错误/警告报告函数的函数指针类型
     */
    typedef void (*report_func)(uint32_t);

    int error_code_set(const uint8_t err_count);
    int error_code_clear(const uint8_t err_count);
    void error_code_clear_all(void);
    uint8_t error_code_get(const uint8_t err_count);
    uint32_t error_code_get_all(void);
    uint32_t warning_code_set(const uint32_t warn);
    uint32_t warning_code_get(void);
    void error_set_report_func(report_func func);
    void warning_set_report_func(report_func func);
    void error_report(void);
    void warning_report(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_ERROR_H__ */
