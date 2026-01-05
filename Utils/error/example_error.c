#include "error.h"

/* 根据需要可以决定是否重定义 error_handle 函数 */
void error_handle(ERRORType err)
{
    if (err.code != 0)
    {
        fprintf(stderr,
                "[ERROR] Code: %d\r\n"
                "        Message: %s\r\n"
                "        File: %s\r\n"
                "        Function: %s\r\n"
                "        Line: %d\r\n",
                err.code,
                err.message,
                err.file,
                err.function,
                err.line);
    }
}

int main()
{
    ERROR_HANDLE(-1, "An error occurred in main function.");
    ERROR_HANDLE(-1, "An error occurred in main function. %s", "Additional info");
    ERROR_CHECK(1 != 1, -2, "Check failed. Values are equal: %d and %d", 1, 1);
    return 0;
}