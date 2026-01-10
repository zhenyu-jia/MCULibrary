/**
 * @file    stdio_redirect.c
 * @brief   实现标准输入、输出、错误的重定向到 UART。
 * @details 本文件提供了将标准输出、标准输入和标准错误输出重定向到指定外设（UART）的功能。
 * @version V1.0.1
 * @date    2024-07-05
 * @author  Jia Zhenyu
 */

#include <stdint.h>
#include "main.h"
#include "usart.h"

#define STDIO_UART huart1  // 标准输入/输出 UART
#define STDERR_UART huart1 // 错误输出 UART，可和 STDIO_UART 同步

/* 标准输出 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
#if (STDIN_ECHO != 0) // 如果启用输入回显
int stdout_putchar(int ch)
{
    HAL_UART_Transmit(&STDIO_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

    return ch;
}
#else
int stdout_putchar(int ch)
{
#define BUF_SIZE 64U
    static uint32_t index = 0U;
    static uint8_t buffer[BUF_SIZE];

    if (index >= BUF_SIZE)
    {
        return (-1);
    }

    buffer[index++] = (uint8_t)ch;

    if ((index == BUF_SIZE) || (ch == '\n'))
    {
        HAL_UART_Transmit(&STDIO_UART, buffer, index, HAL_MAX_DELAY);
        index = 0U;
    }

    return ch;
#undef BUF_SIZE
}
#endif /* STDIN_ECHO */

/* 标准输入 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
int stdin_getchar(void)
{
    uint8_t ch;

    HAL_UART_Receive(&STDIO_UART, &ch, 1, HAL_MAX_DELAY);

    return (int)ch;
}

/* 标准错误 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
int stderr_putchar(int ch)
{
    HAL_UART_Transmit(&STDERR_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

    return ch;
}
