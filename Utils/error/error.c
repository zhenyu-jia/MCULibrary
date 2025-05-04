/**
 * @file error.c
 * @brief 错误处理源文件
 * @author Jia Zhenyu
 * @date 2025-05-05
 * @version 1.0.0
 */

#include <stdio.h>
#include "error.h"

/**
 * @brief 错误处理函数（弱定义）
 *        该函数用于处理错误信息，可以根据需要进行重定义
 * @param err 错误信息结构体
 * @return void
 */
__attribute__((weak)) void error_handle(ERRORType err)
{
    if (err.code != 0)
    {
        // fprintf(stdout, "[Error %d]: %s at \"%s\":[%d] in function [%s]\r\n",
        //         err.code, err.message, err.file, err.line, err.function);

        fprintf(stderr, "[Error %d]: %s at \"%s\":[%d] in function [%s]\r\n",
                err.code, err.message, err.file, err.line, err.function);

        // 彩色输出
        // fprintf(stdout, "\033[31m[Error %d]: %s at \"%s\":[%d] in function [%s]\033[0m\r\n",
        //         err.code, err.message, err.file, err.line, err.function);

        // fprintf(stderr, "\033[31m[Error %d]: %s at \"%s\":[%d] in function [%s]\033[0m\r\n",
        //         err.code, err.message, err.file, err.line, err.function);
    }
}
