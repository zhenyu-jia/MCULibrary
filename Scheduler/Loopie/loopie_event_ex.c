/**
 * @file    loopie_event_ex.c
 * @brief   事件扩展模块，实现事件标志与带回调的事件标志处理功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.1
 */

#include "loopie_event_ex.h"
#include "loopie_config.h"
#include <stdint.h>
#include <string.h>

static EVENT_FLAG event_flag = {0};
static EVENT_FLAG_CB event_flag_cb = {0};

/**
 * @brief 设置标志位
 * @param flag_count 标志编号
 * @retval flag_count
 */
int event_flag_set(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG - 1)
    {
        return -1;
    }
    event_flag.flag[flag_count / 8] |= (1 << (flag_count % 8));
    return flag_count;
}

/**
 * @brief 设置标志位 (带回调函数)
 * @param flag_count 标志编号
 * @retval flag_count
 */
int event_flag_cb_set(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG_CB - 1)
    {
        return -1;
    }
    event_flag_cb.flag[flag_count / 8] |= (1 << (flag_count % 8));
    return flag_count;
}

/**
 * @brief 清除标志位
 * @param flag_count 标志编号
 * @retval flag_count
 */
int event_flag_clear(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG - 1)
    {
        return -1;
    }
    event_flag.flag[flag_count / 8] &= ~(1 << (flag_count % 8));
    return flag_count;
}

/**
 * @brief 清除标志位 (带回调函数)
 * @param flag_count 标志编号
 * @retval flag_count
 */
int event_flag_cb_clear(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG_CB - 1)
    {
        return -1;
    }
    event_flag_cb.flag[flag_count / 8] &= ~(1 << (flag_count % 8));
    return flag_count;
}

/**
 * @brief 获取标志位
 * @param flag_count 标志编号
 * @retval 标志位
 */
uint8_t event_flag_get(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG - 1)
    {
        return 0;
    }
    return (event_flag.flag[flag_count / 8] >> (flag_count % 8)) & 0x01;
}

/**
 * @brief 获取标志位 (带回调函数)
 * @param flag_count 标志编号
 * @retval 标志位
 */
uint8_t event_flag_cb_get(const uint32_t flag_count)
{
    if (flag_count > SCH_EVENT_MAX_FLAG_CB - 1)
    {
        return 0;
    }
    return (event_flag_cb.flag[flag_count / 8] >> (flag_count % 8)) & 0x01;
}

/**
 * @brief 清空所有标志
 */
void event_flag_clear_all(void)
{
    // memset(event_flag.flag, 0, sizeof(event_flag.flag));
    for (size_t i = 0; i < sizeof(event_flag.flag); i++)
    {
        event_flag.flag[i] = 0;
    }
}

/**
 * @brief 清空所有标志 (带回调函数)
 */
void event_flag_cb_clear_all(void)
{
    // memset(event_flag_cb.flag, 0, sizeof(event_flag_cb.flag));
    for (size_t i = 0; i < sizeof(event_flag_cb.flag); i++)
    {
        event_flag_cb.flag[i] = 0;
    }
}

/**
 * @brief 设置回调函数
 * @param callback 回调函数
 */
void event_flag_cb_set_callback(void (*const callback)(uint32_t))
{
    if (!callback)
    {
        return;
    }
    event_flag_cb.callback = callback;
}

/**
 * @brief 逐个触发标志，在主循环调用
 */
void event_flag_cb_process(void)
{
    for (uint32_t i = 0; i < SCH_EVENT_MAX_FLAG_CB; i++)
    {
        if (event_flag_cb_get(i))
        {
            event_flag_cb_clear(i); // 清除该标志
            if (event_flag_cb.callback)
            {
                event_flag_cb.callback(i); // 传入当前标志编号
            }
        }
    }
}
