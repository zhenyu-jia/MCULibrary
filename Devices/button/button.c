/**
 * @file button.c
 * @brief 实现按钮处理的操作函数。
 *
 * 该文件包含用于简单按钮处理的函数的实现。包括按钮的初始化、扫描按钮状态，
 * 并通过回调函数处理按钮状态的变化，以及组合按键的功能。
 *
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 */

#include <string.h>
#include "button.h"

/**
 * @brief 初始化按钮结构体
 *
 * @param hbutton 按钮结构体的指针
 * @param get_button_status 获取当前按钮状态的函数指针
 * @param buttonCallBack 按钮事件的回调函数指针
 * @return 函数执行成功返回 0，否则返回 -1
 */
int button_init(BUTTON *hbutton, uint8_t (*get_button_status)(BUTTON *),
                void (*buttonCallBack)(BUTTON *, uint8_t, uint8_t))
{
    if (hbutton == NULL || get_button_status == NULL || buttonCallBack == NULL)
    {
        return -1;
    }

    memset(hbutton, 0, sizeof(BUTTON));
    hbutton->get_button_status = get_button_status;
    hbutton->button_callback = buttonCallBack;

    return 0;
}

#ifdef BUTTON_COMBINATION
/**
 * @brief 启用组合按键功能
 *
 * @param hbutton 按钮结构体的指针
 * @param combination_index 组合按键的索引，从 0 开始，最多到 7，即最多 8 个组合按键
 * @param combination_mask 组合按键的组合，把相应的位设置为 1
 * @param button_combination_callback 组合按键事件的回调函数指针
 * @return 函数执行成功返回组合按键的索引，否则返回 -1
 */
int button_enable_combination(BUTTON *hbutton, uint8_t combination_index, uint8_t combination_mask,
                              void (*button_combination_callback)(BUTTON *, uint8_t index))
{
    if (hbutton == NULL || combination_index >= BUTTON_COMBINATION_COUNT || combination_mask == 0 || button_combination_callback == NULL)
    {
        return -1;
    }

    hbutton->combination_mask[combination_index] = combination_mask;
    hbutton->button_combination_callback = button_combination_callback;

    return combination_index;
}
#endif

/**
 * @brief 按钮扫描函数，周期性调用
 *
 * 该函数用于扫描按钮的当前状态，并进行滤波处理，以确定按钮是否按下或释放。
 * 扫描到的按钮状态变化通过回调函数通知外部。
 *
 * @param hbutton 按钮结构体的指针
 */
void button_scan(BUTTON *hbutton)
{
    uint8_t current = hbutton->get_button_status(hbutton); // 更新当前状态
    uint8_t asserted_pre = hbutton->asserted;              // 记录上次的按键状态

    hbutton->asserted |= hbutton->previous & current; // 全高则置位，代表按键按下
    hbutton->asserted &= hbutton->previous | current; // 全低则清除，代表按键释放

    hbutton->previous = current; // 更新上次状态

#ifdef BUTTON_COMBINATION // 组合按键功能
    if (hbutton->asserted != 0)
    {
        for (uint8_t i = 0; i < BUTTON_COMBINATION_COUNT; i++)
        {
            if (hbutton->asserted == hbutton->combination_mask[i])
            {
                if (hbutton->asserted == asserted_pre)
                {
                    if (hbutton->combination_ticks < COMBINATION_TICKS)
                    {
                        hbutton->combination_ticks++;
                    }
                    else
                    {
                        if (hbutton->combination_executed == 0)
                        {
                            if (hbutton->button_combination_callback)
                            {
                                hbutton->button_combination_callback(hbutton, i);
                            }

                            hbutton->combination_executed = 1;
                        }
                    }
                }
                else
                {
                    hbutton->combination_ticks = 1; // 状态变化时重新计时
                }

                break; // 一旦匹配成功就不再继续遍历
            }
        }
    }
    else // 所有按键松开时才复位组合标志
    {
        hbutton->combination_ticks = 0;
        hbutton->combination_executed = 0;
    }
#else
    uint8_t pressed = ~asserted_pre & hbutton->asserted;  // 0 → 1:1
    uint8_t released = asserted_pre & ~hbutton->asserted; // 1 → 0:1

    if (hbutton->button_callback)
    {
        hbutton->button_callback(hbutton, pressed, released);
    }
#endif /* BUTTON_COMBINATION */
}
