/**
 * @file    loopie_event.c
 * @brief   事件队列模块，实现事件的投递、处理等功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.1
 */

#include "loopie_event.h"
#include "loopie_config.h"
#include "loopie_critical.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>

static EVENT_QUEUE event_queue = {0}; // 环形事件队列

/**
 * @brief 检查事件队列是否为空
 */
static inline int event_queue_empty(void)
{
    return event_queue.in == event_queue.out;
}

/**
 * @brief 检查事件队列是否已满
 */
static inline int event_queue_full(void)
{
    return (event_queue.in - event_queue.out) == SCH_EVENT_MAX_NUM;
}

/**
 * @brief 初始化
 */
void event_init(void)
{
    event_queue.in = 0;
    event_queue.out = 0;
    memset(event_queue.queue, 0, sizeof(event_queue.queue));
}

/**
 * @brief 投递事件
 * @param pEvent 事件函数
 * @param arg 事件参数
 * @param mode 投递模式
 * @retval 0: 成功，1: 也是成功，但是队列满了覆盖了最旧的事件，-1: 事件函数不能为空，-2: 队列已满，丢弃新事件
 */
int event_post(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode)
{
    if (!pEvent)
    {
        return -1; // 事件函数不能为空
    }

    uint32_t primask = enter_critical();

    int overwritten = 0; // 是否被覆盖

    if (event_queue_full())
    {
        if (mode == EVENT_POST_OVERWRITE)
        {
            // 队列满，覆盖最旧事件
            event_queue.out++; // 丢弃最旧的
            overwritten = 1;
        }
        else
        {
            exit_critical(primask);
            return -2; // 队列已满，丢弃新事件
        }
    }

    uint32_t index = event_queue.in & SCH_EVENT_MASK;
    event_queue.queue[index].pEvent = pEvent;
    event_queue.queue[index].arg = arg;
    event_queue.in++;

    exit_critical(primask);

    return overwritten ? 1 : 0;
}

/**
 * @brief 投递事件，从中断中调用
 * @param pEvent 事件函数
 * @param arg 事件参数
 * @param mode 投递模式
 * @retval 0: 成功，1: 也是成功，但是队列满了覆盖了最旧的事件，-1: 事件函数不能为空，-2: 队列已满，丢弃新事件
 */
int event_post_from_isr(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode)
{
    return event_post(pEvent, arg, mode); // 已经内部加锁
}

/**
 * @brief 投递默认事件
 * @param pEvent 事件函数
 * @retval 0: 成功，-1: 事件函数不能为空，-2: 队列已满，丢弃新事件
 */
int event_post_default(void (*const pEvent)(void *))
{
    return event_post(pEvent, NULL, EVENT_POST_DISCARD);
}

/**
 * @brief 投递默认事件，在中断中调用
 * @param pEvent 事件函数
 * @retval 0: 成功，-1: 事件函数不能为空，-2: 队列已满，丢弃新事件
 */
int event_post_from_isr_default(void (*const pEvent)(void *))
{
    return event_post_from_isr(pEvent, NULL, EVENT_POST_DISCARD);
}

/**
 * @brief 获取事件队列剩余空间
 * @retval 事件队列剩余空间
 */
uint32_t event_queue_free_size(void)
{
    return SCH_EVENT_MAX_NUM - (event_queue.in - event_queue.out);
}

/**
 * @brief 运行事件队列
 */
void event_run(void)
{
    while (!event_queue_empty())
    {
        uint32_t primask = enter_critical();
        uint32_t index = event_queue.out & SCH_EVENT_MASK;
        EVENT evt = event_queue.queue[index];
        event_queue.out++;
        exit_critical(primask);

        if (evt.pEvent)
        {
            evt.pEvent(evt.arg);
        }
    }
}
