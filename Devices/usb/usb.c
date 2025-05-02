/**
 * @file usb.c
 * @brief USB 功能相关的板级支持包 (BSP)
 *
 * 该文件包含用于控制和重置 USB 功能的相关函数实现。通过这些函数，可以实现 USB 设备的重新枚举操作。
 * 避免了每次复位单片机后要重新插拔 USB 的问题。
 *
 * USB 的重新枚举操作通过控制 USB 数据正 (DP) 引脚实现。该过程包括将 DP 引脚拉低一段时间，
 * 然后再将其拉高，从而触发 USB 设备的重新枚举。
 *
 * @note
 * - `USB_Reset`：重置 USB 设备，通过控制 USB DP 引脚实现。
 *
 * @version V1.0.0
 * @date 2024-07-23
 * @author Jia Zhenyu
 *
 * @par Example
 * @code
 * // 触发 USB 重新枚举
 * USB_Reset();
 * // 初始化 USB 设备
 * MX_USB_DEVICE_Init();
 * @endcode
 */

#include "usb.h"
#include "main.h"

/**
 * @brief 重置 USB 设备。
 *
 * 该函数通过控制 USB 数据正 (DP) 引脚来实现 USB 的重新枚举操作。
 * 它首先将 USB DP 引脚设置为推挽输出模式，然后将其拉低 100 毫秒，
 * 最后将其拉高，从而触发 USB 设备的重新枚举。
 *
 * @note 在调用该函数之前，请确保已经启用了 USB DP 引脚所在端口的 GPIO 时钟。
 */
void USB_Reset(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin = USB_DP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(USB_DP_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(USB_DP_GPIO_Port, USB_DP_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(USB_DP_GPIO_Port, USB_DP_Pin, GPIO_PIN_SET);
}
