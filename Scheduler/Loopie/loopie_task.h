/**
 * @file    loopie_task.h
 * @brief   任务管理模块头文件，声明任务的结构体及相关操作函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_TASK_H__
#define __LOOPIE_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    typedef struct __TASK
    {
        void (*pTask)(void *);            // 任务函数
        void *arg;                        // 传入参数
        int8_t priority;                  // 越小优先级越高，负数给调度器相关使用
        volatile uint8_t runFlag : 7;     // 任务运行标志
        volatile uint8_t suspendFlag : 1; // 任务挂起标志
        volatile uint16_t delay;          // 延迟（时标）
        uint16_t cycle;                   // 周期（时标）
        volatile uint32_t last_run_time;  // 上次运行时刻
        volatile uint32_t interval;       // 上一次和这一次之间的实际运行周期
        struct __TASK *next;
    } TASK;

    void task_init(void);
    TASK *task_create(void (*const pTask)(void *), void *const arg, const int8_t priority, const uint16_t delay, const uint16_t cycle);
    int task_delete(TASK *task_handle);
    int task_delete_safe(TASK **task_handle);
    int task_suspend(TASK *task_handle);
    int task_resume(TASK *task_handle);
    void task_update(void);
    void task_run(void);
    int task_get_count(void);
    uint32_t task_get_interval(TASK *task_handle);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_TASK_H__ */
