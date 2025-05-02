/**
 * @file multifunc_button.c
 * @brief 实现按钮处理功能。
 *
 * 该文件包含用于按钮处理的函数的实现，包括按钮去抖动、状态机处理、事件回调等功能。
 * 本文件对按钮处理进行了自定义设计，与原 MultiButton 库相比，进行了以下修改：
 * 1. 变量定义采用了符合自定义的方式。
 * 2. 更改了状态机逻辑，重新设计了按钮状态机。
 * 3. 去掉了查询模式，只支持回调模式。
 * 4. 修改了回调函数的使用方式，统一使用一个回调函数来处理所有按钮事件。
 * 原仓库地址：https://github.com/0x1abin/MultiButton
 *
 * @note
 * - `button_init`：初始化按钮结构体。
 * - `get_button_repeat`：获取按钮重复按下的次数。
 * - `button_start`：启动按钮处理，将按钮添加到处理链表中。
 * - `button_stop`：停止按钮处理，将按钮从处理链表中移除。
 * - `button_ticks`：调用所有按钮的处理函数，更新按钮状态和事件。
 *
 * @version 1.0.0
 * @date 2024-07-30
 * @author Jia Zhenyu
 *
 * @par Example
 * @code
 * void my_button_callback(BUTTON *btn, BUTTON_Event event) {
 *     // 处理按钮事件
 * }
 *
 * BUTTON my_button;
 * button_init(&my_button, my_pin_level_function, my_button_callback);
 * button_start(&my_button);
 *
 * while (1) {
 *     button_ticks(); // 在主循环中定期调用以处理按钮事件
 * }
 * @endcode
 */

#include "multifunc_button.h"

static BUTTON *button_head_handle = NULL;

/**
 * @brief 初始化按钮结构体。
 *
 * 该函数初始化按钮结构体，设置按钮电平读取函数和事件回调函数。
 *
 * @param hbutton 按钮结构体的指针
 * @param pin_level 读取按钮电平状态的函数指针
 * @param button_callback 按钮事件回调函数的函数指针
 */
void button_init(BUTTON *hbutton, uint8_t (*pin_level)(BUTTON *),
                 void (*button_callback)(BUTTON *, BUTTON_Event))
{
    memset(hbutton, 0, sizeof(BUTTON));
    hbutton->get_button_level = pin_level;
    hbutton->button_callback = button_callback;
}

// BUTTON_Event get_button_event(BUTTON *hbutton)
// {
//     return (BUTTON_Event)(hbutton->event);
// }

/**
 * @brief 获取按钮重复按下的次数。
 *
 * 该函数返回按钮被重复按下的次数。
 *
 * @param hbutton 按钮结构体的指针
 * @return 按钮重复按下的次数
 */
uint8_t get_button_repeat(BUTTON *hbutton)
{
    return (uint8_t)(hbutton->repeat);
}

/**
 * @brief 处理按钮去抖动。
 *
 * 该函数对按钮进行去抖动处理，更新按钮的电平状态。
 *
 * @param hbutton 按钮结构体的指针
 */
static void button_debounce(BUTTON *hbutton)
{
    uint8_t button_level = hbutton->get_button_level(hbutton);

    if (button_level != hbutton->button_level)
    {
        if (++(hbutton->debounce_cnt) >= BUTTON_TICKS_DEBOUNCE)
        {
            hbutton->button_level = button_level;
            hbutton->debounce_cnt = 0;
        }
    }
    else
    {
        hbutton->debounce_cnt = 0;
    }
}

/**
 * @brief 处理按钮状态机。
 *
 * 该函数处理按钮的状态机，根据按钮的电平状态和经过的时间更新按钮事件，并调用回调函数。
 *
 * @param hbutton 按钮结构体的指针
 */
static void button_state_machine(BUTTON *hbutton)
{
    switch (hbutton->event)
    {
    case NONE_PRESS:
        if (hbutton->button_level)
        {
            hbutton->event = PRESS_DOWN;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, PRESS_DOWN);
            hbutton->ticks = 0;
            hbutton->repeat = 1;
        }
        break;

    case PRESS_DOWN:
        if (!hbutton->button_level)
        {
            hbutton->event = PRESS_UP;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, PRESS_UP);
            hbutton->ticks = 0;
        }
        else if (hbutton->ticks > BUTTON_TICKS_LONG)
        {
            hbutton->event = LONG_PRESS_START;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, LONG_PRESS_START);
        }
        break;

    case PRESS_UP:
        if (hbutton->button_level)
        {
            hbutton->event = PRESS_DOWN;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, PRESS_DOWN);
            if (hbutton->repeat < PRESS_REPEAT_MAX_NUM)
            {
                hbutton->repeat++;
            }
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, PRESS_REPEAT);
            hbutton->ticks = 0;
        }
        else if (hbutton->ticks > BUTTON_TICKS_SHORT)
        {
            if (hbutton->repeat == 1)
            {
                hbutton->event = SINGLE_CLICK;
                if (hbutton->button_callback)
                    hbutton->button_callback(hbutton, SINGLE_CLICK);
            }
            else if (hbutton->repeat == 2)
            {
                hbutton->event = DOUBLE_CLICK;
                if (hbutton->button_callback)
                    hbutton->button_callback(hbutton, DOUBLE_CLICK);
            }
            hbutton->event = NONE_PRESS;
        }
        break;

    case LONG_PRESS_START:

    case LONG_PRESS_HOLD:
        if (!hbutton->button_level)
        {
            hbutton->event = PRESS_UP;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, PRESS_UP);
            hbutton->event = NONE_PRESS;
        }
        else
        {
            hbutton->event = LONG_PRESS_HOLD;
            if (hbutton->button_callback)
                hbutton->button_callback(hbutton, LONG_PRESS_HOLD);
        }
        break;

    default:
        hbutton->event = NONE_PRESS;
        break;
    }

    hbutton->ticks++;
}

/**
 * @brief 处理单个按钮的去抖动和状态机逻辑。
 *
 * 该函数负责处理按钮的去抖动和状态机逻辑更新。
 *
 * @param hbutton 按钮结构体的指针
 */
static void button_handler(BUTTON *hbutton)
{
    button_debounce(hbutton);
    button_state_machine(hbutton);
}

/**
 * @brief 启动按钮处理。
 *
 * 该函数将按钮添加到处理链表中。若按钮已经存在于链表中，则返回 -1。
 *
 * @param hbutton 按钮结构体的指针
 * @return 成功则返回 0，若按钮已存在则返回 -1
 */
int button_start(BUTTON *hbutton)
{
    BUTTON *target = button_head_handle;
    while (target)
    {
        if (target == hbutton)
        {
            return -1; // already exist.
        }
        target = target->next;
    }
    hbutton->next = button_head_handle;
    button_head_handle = hbutton;
    return 0;
}

/**
 * @brief 停止按钮处理。
 *
 * 该函数将按钮从处理链表中移除。如果按钮不在链表中，则函数不会执行任何操作。
 *
 * @param hbutton 按钮结构体的指针
 */
void button_stop(BUTTON *hbutton)
{
    BUTTON **curr;
    for (curr = &button_head_handle; *curr;)
    {
        BUTTON *entry = *curr;
        if (entry == hbutton)
        {
            *curr = entry->next;
            // free(entry);
            return;
        }
        else
        {
            curr = &entry->next;
        }
    }
}

/**
 * @brief 调用所有按钮的处理函数。
 *
 * 该函数遍历所有按钮并调用其处理函数，以更新按钮的状态和事件。
 */
void button_ticks(void)
{
    BUTTON *target;
    for (target = button_head_handle; target; target = target->next)
    {
        button_handler(target);
    }
}
