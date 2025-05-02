/**
 * @file button.h
 * @brief 按钮处理的头文件。
 *
 * 本文件定义了一个简单按钮处理的数据结构和相关操作函数。用于实现按钮的按下、
 * 释放以及状态变化的处理。
 *
 * 主要功能包括：
 * - 初始化按钮结构体
 * - 扫描按钮状态
 * - 处理按钮状态变化的回调函数
 * - 处理组合按键功能
 *
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

// #define BUTTON_COMBINATION           // 是否使用组合按键功能
#define BUTTON_COMBINATION_COUNT 15U // 组合按键数量
#define COMBINATION_TICKS 10U        // 组合按键的计时器

    /**
     * @brief 按钮结构体定义。
     *
     * 该结构体定义了一个按钮处理单元，用于检测按钮的按下和释放状态。
     */
    typedef struct __BUTTON
    {
        uint8_t asserted; // 按钮状态
        uint8_t previous; // 上一次的按钮状态
#ifdef BUTTON_COMBINATION
        uint8_t combination_mask[BUTTON_COMBINATION_COUNT]; // 每个组合按键的掩码
        uint8_t combination_ticks : 7;                      // 组合按键的计时器
        uint8_t combination_executed : 1;                   // 组合按键是否执行过
#endif
        uint8_t (*get_button_status)(struct __BUTTON *hbutton);                               // 获取按钮状态的回调函数
        void (*button_callback)(struct __BUTTON *hbutton, uint8_t pressed, uint8_t released); // 按钮状态变化的回调函数
#ifdef BUTTON_COMBINATION
        void (*button_combination_callback)(struct __BUTTON *hbutton, uint8_t index); // 组合按键状态变化的回调函数
#endif
    } BUTTON;

    /* 函数声明 */
    int button_init(BUTTON *hbutton, uint8_t (*get_button_status)(BUTTON *),
                    void (*buttonCallBack)(BUTTON *, uint8_t, uint8_t)); /* 初始化按钮 */
    void button_scan(BUTTON *hbutton);                                   /* 扫描按钮状态并处理防抖 */
#ifdef BUTTON_COMBINATION
    int button_enable_combination(BUTTON *hbutton, uint8_t combination_index, uint8_t combination_mask,
                                  void (*button_combination_callback)(BUTTON *, uint8_t index));
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
