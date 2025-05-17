/**
 * @file    loopie_scheduler.h
 * @brief   调度器模块头文件，声明调度器相关接口与中断控制函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.1
 */

#ifndef __LOOPIE_SCHEDULER_H__
#define __LOOPIE_SCHEDULER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "loopie_config.h"
#include "loopie_task.h"
#include "loopie_event.h"
#include "loopie_event_ex.h"
#include "loopie_error.h"
#include "loopie_critical.h"

    void scheduler_set_idle_hook(void (*const hook)(void));
    void scheduler_set_time_func(uint32_t (*const func)(void));
    void scheduler_suspend(void);
    void scheduler_resume(void);
    void scheduler_start(void);
    void scheduler_stop(void);
    void scheduler_restart(void);
    void scheduler_init(void);
    void scheduler_update(void);
    void scheduler_run(void);
    uint32_t scheduler_get_duration(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_SCHEDULER_H__ */
