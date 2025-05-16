/**
 * @file    loopie_event.h
 * @brief   事件队列模块头文件，声明事件队列结构体及相关操作函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_EVENT_H__
#define __LOOPIE_EVENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "loopie_config.h"

    typedef enum
    {
        EVENT_POST_DISCARD = 0, // 默认：丢弃新事件
        EVENT_POST_OVERWRITE    // 覆盖最旧事件
    } EVENT_POST_MODE;

    typedef struct __EVENT
    {
        void (*pEvent)(void *);
        void *arg;
    } EVENT;

    typedef struct
    {
        EVENT queue[SCH_MAX_EVENT_NUM];
        uint32_t in;
        uint32_t out;
    } EVENT_QUEUE;

    void event_init(void);
    int event_post(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode);
    int event_post_from_isr(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode);
    int event_post_default(void (*const pEvent)(void *));
    int event_post_from_isr_default(void (*const pEvent)(void *));
    uint32_t event_queue_free_size(void);
    void event_run(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_EVENT_H__ */
