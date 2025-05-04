/**
 * @file error.h
 * @brief 错误处理头文件
 * @author Jia Zhenyu
 * @date 2025-05-05
 * @version 1.0.0
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

/* 常用错误类型宏定义 */
// 通用错误
#define ERROR_NONE 0                 // 无错误
#define ERROR_OPERATION_FAILED -1    // 通用操作失败
#define ERROR_INVALID_ARGUMENT -2    // 参数错误
#define ERROR_NULL_POINTER -3        // 空指针
#define ERROR_INVALID_STATE -4       // 无效状态
#define ERROR_NOT_INITIALIZED -5     // 未初始化
#define ERROR_ALREADY_INITIALIZED -6 // 已初始化

// 内存和数据错误
#define ERROR_OUT_OF_MEMORY -10    // 内存不足
#define ERROR_OUT_OF_BOUNDS -11    // 越界
#define ERROR_BUFFER_OVERFLOW -12  // 缓冲区溢出
#define ERROR_DIVISION_BY_ZERO -13 // 除零错误
#define ERROR_CRC_MISMATCH -14     // CRC 校验失败

// 设备/硬件相关错误
#define ERROR_DEVICE_NOT_FOUND -20 // 设备未找到
#define ERROR_DEVICE_BUSY -21      // 设备忙
#define ERROR_HW_FAILURE -22       // 硬件故障
#define ERROR_IO -23               // I/O 错误

// 系统/外部错误
#define ERROR_TIMEOUT -30 // 超时

    typedef struct __ERROR
    {
        int code;             // 错误码
        const char *message;  // 错误信息
        const char *file;     // 错误所在的文件名
        const char *function; // 错误所在的函数名
        int line;             // 错误发生的行号
    } ERRORType;

    void error_handle(ERRORType err);

#define ERROR_CHECK(expr, err_code, err_message)                                       \
    do                                                                                 \
    {                                                                                  \
        if (expr)                                                                      \
        {                                                                              \
            ERRORType err = {err_code, err_message, __FILE__, __FUNCTION__, __LINE__}; \
            error_handle(err);                                                         \
            /* return; */                                                              \
        }                                                                              \
    } while (0)

#define ERROR_HANDLE(err_code, err_message)                                        \
    do                                                                             \
    {                                                                              \
        ERRORType err = {err_code, err_message, __FILE__, __FUNCTION__, __LINE__}; \
        error_handle(err);                                                         \
        /* return; */                                                              \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif /* __ERROR_H__ */
