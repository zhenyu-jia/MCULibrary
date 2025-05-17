/**
 * @file    loopie_task.h
 * @brief   任务管理模块头文件，声明任务的结构体及相关操作函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.1.0
 */

#ifndef __LOOPIE_TASK_H__
#define __LOOPIE_TASK_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* 错误码 */
#define ERROR_TASK_QUEUE_FULL -1    // 任务队列已满
#define ERROR_TASK_NOT_FOUND -2     // 任务未找到（无法删除）
#define ERROR_TASK_INVALID_PARAM -3 // 无效的参数（例如无效的函数指针）

    typedef struct __TASK
    {
        void (*pTask)(void *);            // 任务函数
        void *arg;                        // 传入参数
        volatile uint8_t runFlag : 7;     // 任务运行标志
        volatile uint8_t suspendFlag : 1; // 任务挂起标志
        volatile uint16_t delay;          // 延迟（时标）
        uint16_t cycle;                   // 周期（时标）
        volatile uint32_t last_run_time;  // 上次运行时刻
        volatile uint32_t interval;       // 上一次和这一次之间的实际运行周期
    } TASK;

    void task_init(void);
    int task_create(void (*const pTask)(void *), void *const arg, const uint16_t delay, const uint16_t cycle);
    int task_delete(const int task_index);
    int task_suspend(const int task_index);
    int task_resume(const int task_index);
    void task_update(void);
    void task_run(void);
    int task_get_count(void);
    uint32_t task_get_interval(const int task_index);

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_TASK_H__ */
