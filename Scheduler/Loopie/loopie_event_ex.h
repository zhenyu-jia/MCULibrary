/**
 * @file    loopie_event_ex.h
 * @brief   事件扩展模块头文件，声明事件标志与带回调的事件标志相关结构体及操作函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_EVENT_EX_H__
#define __LOOPIE_EVENT_EX_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "loopie_config.h"

    typedef struct
    {
        volatile uint8_t flag[(SCH_MAX_EVENT_FLAG + 7) / 8];
    } EVENT_FLAG;

    typedef struct
    {
        volatile uint8_t flag[(SCH_MAX_EVENT_FLAG_CB + 7) / 8];
        void (*callback)(uint32_t);
    } EVENT_FLAG_CB;

    int event_flag_set(const uint32_t flag_count);
    int event_flag_cb_set(const uint32_t flag_count);
    int event_flag_clear(const uint32_t flag_count);
    int event_flag_cb_clear(const uint32_t flag_count);
    uint8_t event_flag_get(const uint32_t flag_count);
    uint8_t event_flag_cb_get(const uint32_t flag_count);
    void event_flag_clear_all(void);
    void event_flag_cb_clear_all(void);
    void event_flag_cb_set_callback(void (*const callback)(uint32_t));
    void event_flag_cb_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_EVENT_EX_H__ */
