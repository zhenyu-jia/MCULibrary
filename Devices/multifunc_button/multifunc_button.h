/**
 * @file multifunc_button.h
 * @brief 按钮处理的头文件。
 *
 * 本文件定义了用于按钮处理的结构体和函数，包括按钮初始化、事件检测、按键状态读取等功能。
 * 按钮处理功能包括支持短按、长按、单击、双击、重复按下等多种按键事件，并提供了去抖动和定时功能。
 *
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 */

#ifndef __MULTIFUNC_BUTTON_H__
#define __MULTIFUNC_BUTTON_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

// #define BUTTON_TICKS_INTERVAL 5U // 时间间隔即按键扫描周期 (ms)
#define BUTTON_TICKS_DEBOUNCE 3U // 消抖时间 MAX 7 (0 ~ 7, 三个二进制位)(KEY_TICKS_INTERVAL)
#define BUTTON_TICKS_SHORT 60U   // 短按时间 (KEY_TICKS_INTERVAL)
#define BUTTON_TICKS_LONG 200U   // 长按时间 (KEY_TICKS_INTERVAL)
#define PRESS_REPEAT_MAX_NUM 15U // 按键重复按下最大次数

    /**
     * @brief 按钮事件类型定义
     *
     * 该枚举类型定义了不同的按钮事件，用于表示按钮的状态变化。
     */
    typedef enum
    {
        NONE_PRESS = 0,   // 表示按钮没有被按下的事件
        PRESS_DOWN,       // 表示按钮被按下的事件
        PRESS_UP,         // 表示按钮被释放的事件
        PRESS_REPEAT,     // 表示按钮被重复按下的事件
        SINGLE_CLICK,     // 表示按钮被单击的事件
        DOUBLE_CLICK,     // 表示按钮被双击的事件
        LONG_PRESS_START, // 表示按钮长按开始的事件
        LONG_PRESS_HOLD   // 表示按钮长按持续的事件
    } BUTTON_Event;

    /**
     * @brief 按钮结构体定义
     *
     * 该结构体定义了按钮的状态、事件以及相关操作函数。
     */
    typedef struct __BUTTON
    {
        uint16_t ticks;     // 用于记录按钮按下或释放后经过的时间 (BUTTON_TICKS_INTERVAL)
        uint8_t repeat : 4; // 用于记录按钮重复按下的次数，最多可以表示 15 次
        uint8_t event : 4;  // 表示当前按钮的事件类型，使用 BUTTON_Event 枚举类型来表示。
        // uint8_t   pre_event : 4;  // 事件缓存
        uint8_t debounce_cnt : 3;                              // 用于按钮去抖动处理的计数器，最多可以表示 7 次
        uint8_t button_level : 1;                              // 表示当前按钮的电平状态，0 表示低电平，1 表示高电平
        uint8_t (*get_button_level)(struct __BUTTON *hbutton); // 读取按钮的当前电平状态：普通按钮（读 IO）、矩阵键盘 or 其他键盘、遥控器、屏幕上的触摸区域、ADC 按键
        void (*button_callback)(struct __BUTTON *hbutton, BUTTON_Event button_status);
        struct __BUTTON *next; // 指向下一个按钮的指针，用于构建按钮链表
    } BUTTON;

    void button_init(BUTTON *hbutton, uint8_t (*pin_level)(BUTTON *),
                     void (*button_callback)(BUTTON *, BUTTON_Event)); /* 初始化按钮 */
    // BUTTON_Event get_button_event(BUTTON *hbutton);
    uint8_t get_button_repeat(BUTTON *hbutton); /* 获取按钮重复按下的次数 */
    int button_start(BUTTON *hbutton);          /* 启动按钮处理 */
    void button_stop(BUTTON *hbutton);          /* 停止按钮处理 */
    void button_ticks(void);                    /* 按钮扫描定时器 */

#ifdef __cplusplus
}
#endif

#endif /* __MULTIFUNC_BUTTON_H__ */
