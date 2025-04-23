/**
 * @file log.c
 * @brief 实现日志记录功能的源文件。
 *
 * 该文件包含了日志记录的功能实现，包括设置日志级别、输出日志消息以及
 * 设置自定义的日志输出函数。日志级别包括调试 (DEBUG)、信息 (INFO)、警告 (WARN) 和
 * 错误 (ERROR)。在定义了 `_DEBUG` 宏的情况下，日志记录功能将被启用。
 *
 * 文件中的主要功能包括：
 * - `log_message`：记录日志消息，输出日志级别、函数名、行号及格式化的日志内容。
 * - `set_log_output`：设置自定义的日志输出函数，以便用户可以定制日志输出方式。
 * - `log_level_get`：根据日志级别获取对应的日志级别字符串（仅在 `_DEBUG` 被定义时有效）。
 *
 * @note
 * - `log_message`：记录日志并输出到控制台或通过自定义函数输出。
 * - `set_log_output`：设置日志输出函数，可以替换默认的 `printf`。
 * - `log_level_get`：根据日志级别返回字符串描述，仅在调试模式下有效。
 *
 * @version 1.1.0
 * @date 2025-04-21
 * @author [Jia Zhenyu]
 *
 * @par Example
 * @code
 * // 设置自定义日志输出函数
 * void custom_log_output(const char *log)
 * {
 *     // 自定义日志处理，例如写入文件
 *     FILE *file = fopen("log.txt", "a");
 *     if (file)
 *     {
 *         fputs(log, file);
 *         fclose(file);
 *     }
 * }
 * set_log_output(custom_log_output);
 *
 * // 使用日志记录功能
 * log_printf(LOG_INFO, "This is an informational message.");
 * log_printf(LOG_ERROR, "This is an error message with code %d.", error_code);
 * @endcode
 */

#include "log.h"

/**
 * @brief 当前日志输出函数指针
 *
 * 这是一个指向自定义日志输出函数的指针。如果设置了自定义函数，
 * 日志将通过这个函数输出；否则，使用默认的 `printf` 输出。
 */
static log_output_func log_output = NULL;

/**
 * @brief 获取日志级别的字符串表示
 *
 * 这个函数根据日志级别返回相应的字符串描述。仅在 `_DEBUG` 被定义时有效。
 *
 * @param[in] level 日志级别
 * @return 日志级别的字符串描述
 */
#ifdef _DEBUG
static const char *log_level_get(LOGLEVEL level)
{
    switch (level)
    {
    case LOG_DEBUG:
        return "DEBUG";
    case LOG_INFO:
        return " INFO";
    case LOG_WARN:
        return " WARN";
    case LOG_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}
#endif /* _DEBUG */

/**
 * @brief 设置自定义日志输出函数
 *
 * 这个函数允许用户设置一个自定义的日志输出函数。设置之后，日志消息将
 * 通过该函数输出，而不是使用默认的 `printf`。
 *
 * @param[in] func 自定义的日志输出函数
 */
void set_log_output(log_output_func func)
{
    log_output = func;
}

/**
 * @brief 记录日志消息
 *
 * 记录日志消息，输出日志级别、函数名、行号以及格式化的日志内容。
 * 仅在 `_DEBUG` 被定义时有效。如果设置了自定义日志输出函数，日志消息
 * 将通过该函数输出，否则使用 `printf` 输出。
 *
 * @param[in] level 日志级别
 * @param[in] fun 函数名
 * @param[in] line 行号
 * @param[in] fmt 格式化字符串
 * @param[in] ... 格式化字符串的可变参数
 */
void log_message(LOGLEVEL level, const char *fun, const int line, const char *fmt, ...)
{
#ifdef _DEBUG
    if ((int)level < (int)LOG_LEVEL)
    {
        return;
    }

    va_list arg;

    va_start(arg, fmt);
    int size = vsnprintf(NULL, 0, fmt, arg);
    va_end(arg);
    if (size < 0)
    {
        return; // 错误处理
    }

    char buf[size + 1];
    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);

    char log_buf[LOG_BUF_SIZE];
    
#ifdef ANSI_ESCAPE_SEQUENCES
    /* 根据日志级别添加颜色 */
    const char *color_start = "";       // 默认没有颜色
    const char *color_end = "\033[0m";  // 默认颜色重置
    
    switch (level)
    {
    case LOG_DEBUG:
        color_start = "\033[34m";  // 蓝色
        break;
    case LOG_INFO:
        color_start = "\033[32m";  // 绿色
        break;
    case LOG_WARN:
        color_start = "\033[33m";  // 黄色
        break;
    case LOG_ERROR:
        color_start = "\033[31m";  // 红色
        break;
    default:
        break;
    }

    // 拼接日志消息，带颜色
    snprintf(log_buf, sizeof(log_buf), "%s[%s] [Fun:%s Line:%d] %s%s",
             color_start, log_level_get(level), fun, line, buf, color_end);
#else
    /* 没有颜色输出，直接拼接日志消息 */
    snprintf(log_buf, sizeof(log_buf), "[%s] [Fun:%s Line:%d] %s",
             log_level_get(level), fun, line, buf);
#endif /* ANSI_ESCAPE_SEQUENCES */

    if (log_output)
    {
        log_output(log_buf);
    }
    else
    {
        LOG_PRINTF("%s", log_buf);
    }
#endif /* _DEBUG */
}
