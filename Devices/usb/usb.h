/**
 * @file usb.h
 * @brief USB 功能相关的板级支持包 (BSP) 头文件
 *
 * 本文件声明了用于控制和重置 USB 功能的相关函数。通过这些函数，可以实现 USB 设备的重新枚举操作，
 * 从而在某些情况下解决 USB 连接问题或重新初始化 USB 设备。
 *
 * 主要功能包括：
 * - 重置 USB 设备，通过控制 USB DP 引脚实现重新枚举
 *
 * @version V1.0.0
 * @date 2024-07-23
 * @author Jia Zhenyu
 */

#ifndef __USB_H__
#define __USB_H__

#ifdef __cplusplus
extern "C"
{
#endif

    void USB_Reset(void); /* 重置 USB 设备 */

#ifdef __cplusplus
}
#endif

#endif /* __USB_H__ */
