/**
 * @file bsp_stdio.c
 * @brief 标准输入输出重定向实现文件
 *
 * 本文件实现了将标准输入、输出和错误重定向到 UART 或 USB 的功能。根据宏定义，
 * 可以选择通过 UART 或 USB 实现标准输入输出功能。
 *
 * - 定义 `STDIO_USER_UART` 时，通过 UART 实现标准输入输出。
 * - 定义 `STDIO_USER_USB` 时，通过 USB 实现标准输入输出。
 *
 * 示例用法：
 * - 要使用 UART 进行标准输入输出，定义宏 `STDIO_USER_UART`。
 * - 要使用 USB 进行标准输入输出，定义宏 `STDIO_USER_USB`。
 * - 若同时定义了 `STDIO_USER_UART` 和 `STDIO_USER_USB`，优先使用 `STDIO_USER_UART`。
 *
 * @version 1.0
 * @date 2024-07-24
 * @author [Jia Zhenyu]
 *
 * @note
 * - 在使用 `USB_Transmit` 函数时，确保 USB 设备已经初始化。
 * - 在使用 `HAL_UART_Transmit` 和 `HAL_UART_Receive` 函数时，确保 UART 设备已经初始化。
 *
 * @par Example
 * @code
 *  // 定义 STDIO_USER_UART 或 STDIO_USER_USB 进行选择
 *  // 例如，定义 STDIO_USER_UART 以使用 UART 进行标准输入输出
 * @endcode
 *
 * @defgroup STDIO_User 标准输入输出重定向模块
 * @{
 */



#include "stm32f1xx_hal.h"
#include <stdio.h>

#include "RTE_Components.h" // 不用可以注释掉
#define STDIO_USER_USB
//#define STDIO_USER_UART

#ifdef STDIO_USER_UART
#include "main.h"
#include "usart.h"
#endif /* STDIO_USER_UART */

#ifdef STDIO_USER_USB
#include "usbd_cdc_if.h"
#endif

#if (defined RTE_Compiler_IO_STDIN_User) || (defined RTE_Compiler_IO_STDOUT_User) || (defined RTE_Compiler_IO_STDERR_User)

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
    return (int)ch;
#endif /* STDIO_USER_USB */
}
#endif /* RTE_Compiler_IO_STDIN_User */

#ifdef RTE_Compiler_IO_STDOUT_User
#ifdef STDIO_USER_USB
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

#else /* RTE_Compiler_IO_STDIN_User || RTE_Compiler_IO_STDOUT_User || RTE_Compiler_IO_STDERR_User */
/**
 * 加入以下代码,支持 printf 函数,而不需要选择 use MicroLIB
 */
#ifndef __MICROLIB /* 不用 MicroLIB */

/* 确保没有从 C 库链接使用半主机的函数 */
#pragma import(__use_no_semihosting)

/* 标准库需要的支持函数 */
struct __FILE
{
    int handle;
};

FILE __stdout; /* FILE is typedef’d in stdio.h */
FILE __stdin;

/* 定义 _ttywrch */
void _ttywrch(int ch)
{
    ch = ch;
}

/* 定义 _sys_exit() 以避免使用半主机模式 */
void _sys_exit(int return_code)
{
    return_code = return_code;
}

#endif /* __MICROLIB */

#ifdef STDIO_USER_USB
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

/*!
   \brief      重定义 fputc 函数
   \param[in]  ch:
   \param[in]  f:
   \param[out] none
   \retval     none
*/
int fputc(int ch, FILE *f)
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

/*!
   \brief      重定义 fgetc 函数
   \param[in]  ch:
   \param[in]  f:
   \param[out] none
   \retval     none
*/
int fgetc(FILE *f)
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
    return (int)ch;
#endif /* STDIO_USER_USB */
}
#endif /* RTE_Compiler_IO_STDIN_User || RTE_Compiler_IO_STDOUT_User || RTE_Compiler_IO_STDERR_User */
