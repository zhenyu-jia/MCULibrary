/**
 * @file log.h
 * @brief 日志功能的头文件。
 *
 * 本文件定义了日志系统的接口，包括日志级别、日志消息输出功能以及日志输出回调函数的设置。
 * 提供了灵活的日志记录功能，支持不同的日志级别以及自定义日志输出方式。
 *
 * @note
 * - 日志级别的定义包括 `LOG_DEBUG`、`LOG_INFO`、`LOG_WARN` 和 `LOG_ERROR`。
 * - 通过 `set_log_output` 函数可以设置自定义的日志输出函数。
 * - 在编译时可以定义 `_DEBUG` 来启用调试信息输出。
 *
 * @version 1.1.1
 * @date 2025-04-22
 * @author [Jia Zhenyu]
 */

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdarg.h>

/* 定义调试模式 */
#define _DEBUG

/* 定义宏 LOG_LEVEL */
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#define ANSI_ESCAPE_SEQUENCES       /* 是否使用 ANSI 转义序列，即带颜色的输出 */
#define LOG_BUF_SIZE    256         /* 输出 buffer 大小 */

/**
 * @brief 日志级别的枚举类型
 */
typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
} LOGLEVEL;

/**
 * @brief 日志输出函数类型
 *
 * 用户可以定义一个函数，该函数接受一个字符串，并处理日志输出。
 *
 * @param[in] msg 日志消息字符串
 */
typedef void (*log_output_func)(const char *);

/**
 * @brief 记录日志消息
 *
 * 根据指定的日志级别、函数名、行号和格式化字符串记录日志消息。
 *
 * @param[in] level 日志级别
 * @param[in] fun 函数名
 * @param[in] line 行号
 * @param[in] fmt 格式化字符串
 * @param[in] ... 格式化字符串的可变参数
 */
void log_message(LOGLEVEL level, const char *fun, const int line, const char *fmt, ...);

/**
 * @brief 记录格式化日志消息的宏
 *
 * 这个宏用来简化日志记录的调用，自动传递当前函数名和行号。
 *
 * @param[in] level 日志级别
 * @param[in] fmt 格式化字符串
 * @param[in] ... 格式化字符串的可变参数
 */
#define log_printf(level, fmt...) log_message(level, __FUNCTION__, __LINE__, fmt)

/**
 * @brief 设置自定义日志输出函数
 *
 * 用户可以通过这个函数设置一个自定义的日志输出函数。设置之后，日志消息将通过该
 * 函数输出，而不是使用默认的输出方式。
 *
 * @param[in] func 自定义的日志输出函数
 */
void set_log_output(log_output_func func);

#endif /* __LOG_H__ */
