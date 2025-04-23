#include "log.h"
#include <stdio.h>

/* 自定义日志输出函数：输出到控制台 */
void log_to_console(const char *log)
{
    printf("[Console] %s", log);
}

/* 自定义日志输出函数：输出到文件 */
void log_to_file(const char *log)
{
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

/* 自定义日志输出函数：同时输出到控制台和文件 */
void log_to_multiple_targets(const char *log)
{
    // 输出到控制台
    printf("[Multiple Targets] %s", log);

    // 输出到文件
    FILE *file = fopen("log.txt", "a");
    if (file)
    {
        fputs(log, file);
        fclose(file);
    }
}

int main(void)
{
    // 设置日志输出到控制台
    set_log_output(log_to_console);
    log_printf(LOG_INFO, "This is an informational message.\n");

    // 设置日志输出到文件
    set_log_output(log_to_file);
    log_printf(LOG_WARN, "This is a warning message.\n");

    // 设置日志输出到多个目标
    set_log_output(log_to_multiple_targets);
    log_printf(LOG_ERROR, "This is an error message.\n");

    // 调试日志（需要定义 _DEBUG 宏）
    log_printf(LOG_DEBUG, "This is a debug message.\n");

    return 0;
}