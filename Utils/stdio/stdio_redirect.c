/**
 * @file    stdio_redirect.c
 * @brief   实现标准输入输出（stdio）的重定向到 UART 或 USB。
 * @details 本文件提供了将标准输出、标准输入和标准错误输出重定向到指定外设（UART 或 USB）的功能。
 *          该实现支持 GCC 和 MDK 编译器。
 * @version V1.0.0
 * @date    2024-07-03
 * @author  Jia Zhenyu
 */

#include <stdio.h>
#include "main.h"

#define _UART 0
#define _USB 1
#define _GCC 0
#define _MDK 1

// 以下两个宏按照实际情况选择
#define STDIO_REDIRECT_PERIPHERALS _UART // _UART or _USB
#define STDIO_REDIRECT_COMPILER _MDK     // _GCC or _MDK

// 根据选择的外设和编译器选择对应的头文件
#if (STDIO_REDIRECT_PERIPHERALS == _UART)
#include "usart.h"
#define STDIO_REDIRECT_UART huart1
#elif (STDIO_REDIRECT_PERIPHERALS == _USB)
#include "usbd_cdc_if.h"
#else
#error "Please select the peripherals for stdio redirect."
#endif /* STDIO_REDIRECT_PERIPHERALS */

#if (STDIO_REDIRECT_COMPILER == _MDK)
#include "RTE_Components.h"
#endif /* STDIO_REDIRECT_COMPILER */

#if (STDIO_REDIRECT_PERIPHERALS == _USB)
/**
 * @brief  通过 USB 发送数据
 * @param  pData: 要发送的数据指针
 * @param  Size:  数据长度
 * @param  Timeout: 超时时间
 * @retval HAL 状态
 */
HAL_StatusTypeDef USB_Transmit(const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    // USB 阻塞发送
    uint32_t tickstart = 0U;

    if ((pData == NULL) || (Size == 0U))
    {
        return HAL_ERROR;
    }

    tickstart = HAL_GetTick();

    while (USBD_OK != CDC_Transmit_FS((uint8_t *)pData, Size))
    {
        if (Timeout != HAL_MAX_DELAY)
        {
            if (((HAL_GetTick() - tickstart) > Timeout) || (Timeout == 0U))
            {
                return HAL_TIMEOUT;
            }
        }
    }
    return HAL_OK;
}
#endif /* STDIO_REDIRECT_PERIPHERALS */

// 根据编译器进行不同的处理
#if (STDIO_REDIRECT_COMPILER == _MDK)
#define PUTCHAR_PROTOTYPE int stdout_putchar(int ch)
#define GETCHAR_PROTOTYPE int stdin_getchar(void)
#define ERRCHAR_PROTOTYPE int stderr_putchar(int ch)
#elif (STDIO_REDIRECT_COMPILER == _GCC)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#define ERRCHAR_PROTOTYPE int __io_errchar(int ch)
#endif /* STDIO_REDIRECT_COMPILER */

// 标准输出
PUTCHAR_PROTOTYPE
{
#if (STDIO_REDIRECT_PERIPHERALS == _UART)
    if (HAL_UART_Transmit(&STDIO_REDIRECT_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY))
    {
        Error_Handler();
    }
    return (int)ch;
#elif (STDIO_REDIRECT_PERIPHERALS == _USB)
    if (USB_Transmit((uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    return (int)ch;
#else
    return EOF;
#endif /* STDIO_REDIRECT_PERIPHERALS */
}

// 标准输入
GETCHAR_PROTOTYPE
{
    uint8_t ch;
#if (STDIO_REDIRECT_PERIPHERALS == _UART)
    if (HAL_OK == HAL_UART_Receive(&STDIO_REDIRECT_UART, &ch, 1, HAL_MAX_DELAY))
    {
        HAL_UART_Transmit(&STDIO_REDIRECT_UART, &ch, 1, HAL_MAX_DELAY); // 回显

        if (ch == '\r')
        {
            HAL_UART_Transmit(&STDIO_REDIRECT_UART, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
        }

        return (int)ch;
    }
    else
    {
        return EOF; // 表示没有接收到字符
    }
#elif (STDIO_REDIRECT_PERIPHERALS == _USB)
    // USB 输入处理代码可以在这里添加
    return (int)ch;
#else
    return EOF;
#endif /* STDIO_REDIRECT_PERIPHERALS */
}

// 标准错误输出
ERRCHAR_PROTOTYPE
{
#if (STDIO_REDIRECT_PERIPHERALS == _UART)
    if (HAL_UART_Transmit(&STDIO_REDIRECT_UART, (uint8_t *)&ch, 1, HAL_MAX_DELAY))
    {
        Error_Handler();
    }
    return (int)ch;
#elif (STDIO_REDIRECT_PERIPHERALS == _USB)
    if (USB_Transmit((uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    return (int)ch;
#else
    return EOF;
#endif /* STDIO_REDIRECT_PERIPHERALS */
}
