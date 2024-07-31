/**
 * @file stdio_user.c
 * @brief 标准输入输出重定向实现文件
 *
 * 本文件包含了将标准输入输出重定向到 UART 或 USB 的功能实现。
 * 可以通过定义 STDIO_USER_UART 或 STDIO_USER_USB 宏来选择重定向的设备。
 * - 定义 STDIO_USER_UART 时，通过 UART 实现标准输入输出。
 * - 定义 STDIO_USER_USB 时，通过 USB 实现标准输入输出。
 *
 * @version 1.0
 * @date 2024-07-24
 * @author [Jia Zhenyu]
 *
 * @note
 * - `stdin_getchar`：从标准输入获取一个字符。
 * - `stdout_putchar`：向标准输出输出一个字符。
 * - `stderr_putchar`：向标准错误输出一个字符。
 * - `USB_Transmit`：通过 USB 发送数据。
 *
 * @par Example
 * @code
 *  None
 * @endcode
 *
 * @defgroup STDIO_User 标准输入输出重定向模块
 * @{
 */

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include "RTE_Components.h"

// #define STDIO_USER_USB
#define STDIO_USER_UART

#ifdef STDIO_USER_UART
#include "main.h"
#include "usart.h"
#endif /* STDIO_USER_UART */

#ifdef RTE_Compiler_IO_STDIN_User
/**
 * @brief  获取输入字符
 * @retval 获取的字符
 */
int stdin_getchar(void)
{
    uint8_t ch;
#ifdef STDIO_USER_UART
    if (HAL_OK == HAL_UART_Receive(&huart_std, &ch, 1, HAL_MAX_DELAY))
    {
        HAL_UART_Transmit(&huart_std, &ch, 1, HAL_MAX_DELAY); // 回显
        return (int)ch;
    }
    else
    {
        return EOF; // 表示没有接收到字符
    }
#endif /* STDIO_USER_UART */
#ifdef STDIO_USER_USB
    // USB 输入处理代码可以在这里添加
#endif /* STDIO_USER_USB */
}
#endif /* RTE_Compiler_IO_STDIN_User */

#ifdef RTE_Compiler_IO_STDOUT_User
#ifdef STDIO_USER_USB
#include "usbd_cdc_if.h"
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
#endif /* STDIO_USER_USB */

/**
 * @brief  输出字符到标准输出
 * @param  ch: 要输出的字符
 * @retval 输出的字符
 */
int stdout_putchar(int ch)
{
#ifdef STDIO_USER_UART
    if (HAL_UART_Transmit(&huart_std, (uint8_t *)&ch, 1, HAL_MAX_DELAY))
    {
        Error_Handler();
    }
#endif /* STDIO_USER_UART */

#ifdef STDIO_USER_USB
    if (USB_Transmit((uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
#endif /* STDIO_USER_USB */
    return (int)ch;
}
#endif /* RTE_Compiler_IO_STDOUT_User */

#ifdef RTE_Compiler_IO_STDERR_User
/**
 * @brief  输出字符到标准错误
 * @param  ch: 要输出的字符
 * @retval 输出的字符
 */
int stderr_putchar(int ch)
{
#ifdef STDIO_USER_UART
    if (HAL_UART_Transmit(&huart_std, (uint8_t *)&ch, 1, HAL_MAX_DELAY))
    {
        Error_Handler();
    }
#endif /* STDIO_USER_UART */

#ifdef STDIO_USER_USB
    if (USB_Transmit((uint8_t *)&ch, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
#endif /* STDIO_USER_USB */

    return (int)ch;
}
#endif /* RTE_Compiler_IO_STDERR_User */
