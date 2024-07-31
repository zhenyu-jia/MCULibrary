/**
 * @file bsp_button_simple.c
 * @brief 实现简单按钮处理的操作函数。
 *
 * 该文件包含用于简单按钮处理的函数的实现。包括按钮的初始化、扫描按钮状态，
 * 并通过回调函数处理按钮状态的变化。
 *
 * @version 1.0
 * @date 2024-07-30
 * @author [Jia Zhenyu]
 *
 * @par Example
 * @code
 * BUTTON_s button;
 * button_s_init(&button, get_button_status_function, button_event_callback_function);
 * while (1) {
 *     button_s_scan(&button);
 *     // 其他处理
 * }
 * @endcode
 */

#include "bsp_button_simple.h"

/**
 * @brief 初始化按钮结构体
 *
 * @param hbutton 按钮结构体的指针
 * @param buttonStatus 获取当前按钮状态的函数指针
 * @param buttonCallBack 按钮事件的回调函数指针
 */
void button_s_init(BUTTON_s *hbutton, uint8_t (*buttonStatus)(BUTTON_s *),
                   void (*buttonCallBack)(BUTTON_s *, uint8_t, uint8_t))
{
    memset(hbutton, 0, sizeof(BUTTON_s));
    hbutton->get_button_status = buttonStatus;
    hbutton->button_callback = buttonCallBack;
}

/**
 * @brief 按钮扫描函数，周期性调用
 *
 * 该函数用于扫描按钮的当前状态，并进行滤波处理，以确定按钮是否按下或释放。
 * 扫描到的按钮状态变化通过回调函数通知外部。
 *
 * @param hbutton 按钮结构体的指针
 */
void button_s_scan(BUTTON_s *hbutton)
{
    // 获取当前按键的实际值
    uint8_t current = hbutton->get_button_status(hbutton);

    // 滤波得到一个理想按键
    uint8_t pre_depressed = hbutton->depressed;        /* 保存上一次的滤波后的值 */
    hbutton->depressed |= hbutton->previous & current; /* 重新计算滤波后的值 */
    hbutton->depressed &= hbutton->previous | current;

    // 更新按键上一次的实际值
    hbutton->previous = current;

    // 根据理想按键的值判断按键状态
    uint8_t pressed = hbutton->depressed & (hbutton->depressed ^ pre_depressed);
    uint8_t released = pre_depressed & (hbutton->depressed ^ pre_depressed);

    hbutton->button_callback(hbutton, pressed, released);
}
