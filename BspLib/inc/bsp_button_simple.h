/**
 * @file bsp_button_simple.h
 * @brief 简单按钮处理的头文件。
 *
 * 本文件定义了一个简单按钮处理的数据结构和相关操作函数。用于实现按钮的按下、
 * 释放以及状态变化的处理。
 *
 * 主要功能包括：
 * - 初始化按钮结构体
 * - 扫描按钮状态
 * - 处理按钮状态变化的回调函数
 *
 * @version 1.0
 * @date 2024-07-30
 * @author [Jia Zhenyu]
 */

#ifndef __BSP_BUTTON_SIMPLE_H__
#define __BSP_BUTTON_SIMPLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

    /**
     * @brief 按钮结构体定义。
     *
     * 该结构体定义了一个按钮处理单元，用于检测按钮的按下和释放状态。
     */
    typedef struct __BUTTON_s
    {
        uint8_t depressed;                                        /* 滤波后的值 */
        uint8_t previous;                                         /* 上一次的实际值 */
        uint8_t (*get_button_status)(struct __BUTTON_s *hbutton); /* 用于读取按钮的当前电平状态 */
        void (*button_callback)(struct __BUTTON_s *hbutton, uint8_t pressed, uint8_t released);
    } BUTTON_s;

    void button_s_init(BUTTON_s *hbutton, uint8_t (*buttonStatus)(BUTTON_s *),
                       void (*buttonCallBack)(BUTTON_s *, uint8_t, uint8_t)); /* 初始化按钮 */
    void button_s_scan(BUTTON_s *hbutton);                                    /* 扫描按钮状态并处理防抖 */

#ifdef __cplusplus
}
#endif

#endif /* __BSP_BUTTON_SIMPLE_H__ */
