/**
 * @file    loopie_scheduler.c
 * @brief   调度器模块，实现任务调度、事件处理、错误与警告报告等功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#include "loopie_scheduler.h"
#include <stddef.h>

static void (*idle_hook)(void) = NULL;           // 空闲钩子
uint32_t (*get_system_ticks)(void) = NULL;       // 获取系统时间
static volatile uint32_t scheduler_duration = 0; // 调度器运行时长
static volatile int scheduler_lock = 0;          // 锁的计数器，用于支持嵌套调用
static volatile int scheduler_running = 0;       // 调度器运行状态

static int scheduler_is_running(void);

/**
 * @brief 设置空闲钩子
 * @param hook 空闲钩子函数
 */
void scheduler_set_idle_hook(void (*const hook)(void))
{
    idle_hook = hook;
}
// 例子
// void user_idle_handler(void)
// {
//     // 可用于省电、喂狗、空转等
//     __WFI(); // 等待中断，省电
// }
// scheduler_set_idle_hook(user_idle_handler);

/**
 * @brief 设置获取系统时间的函数
 * @param func 系统时间函数
 */
void scheduler_set_time_func(uint32_t (*const func)(void))
{
    get_system_ticks = func;
}

/**
 * @brief 暂停调度器
 */
void scheduler_suspend(void)
{
    uint32_t primask = enter_critical();
    scheduler_lock++;
    exit_critical(primask);
}

/**
 * @brief 恢复调度器
 */
void scheduler_resume(void)
{
    uint32_t primask = enter_critical();
    if (scheduler_lock > 0)
    {
        scheduler_lock--;
    }
    exit_critical(primask);
}

/**
 * @brief 启动调度器
 */
void scheduler_start(void)
{
    scheduler_running = 1;
    scheduler_lock = 0; // 初始化锁计数器
}

/**
 * @brief 停止调度器
 */
void scheduler_stop(void)
{
    scheduler_running = 0;
}

/**
 * @brief 重启调度器
 */
void scheduler_restart(void)
{
    scheduler_stop();
    scheduler_start();
}

/**
 * @brief 判断调度器是否可运行
 * @return 可运行返回 1，否则返回 0
 */
static int scheduler_is_running(void)
{
    return scheduler_running && scheduler_lock == 0;
}

/**
 * @brief 调度器初始化
 */
void scheduler_init(void)
{
    task_init();

#ifdef SCH_EVENT // 事件处理
    event_init();
#endif // SCH_EVENT
}

/**
 * @brief 调度器更新
 */
void scheduler_update(void)
{
    task_update();
}
/**
 * @brief 调度器运行
 */
void scheduler_run(void)
{
    uint32_t start_ticks = get_system_ticks ? get_system_ticks() : 0;

    if (!scheduler_is_running())
    {
        return;
    }

#ifdef SCH_EVENT // 事件处理
    event_run();
#endif // SCH_EVENT

#ifdef SCH_EVENT_EX // 事件扩展处理
    event_flag_cb_process();
#endif // SCH_EVENT_EX

    task_run();

#ifdef SCH_REPORT_ERRORS // 报告错误
    error_report();
#endif // SCH_REPORT_ERRORS

#ifdef SCH_REPORT_WARNINGS // 报告警告
    warning_report();
#endif // SCH_REPORT_WARNINGS

    // 所有任务执行完后执行 idle 钩子
    if (idle_hook)
    {
        idle_hook();
    }

    uint32_t end_ticks = get_system_ticks ? get_system_ticks() : 0;
    scheduler_duration = end_ticks - start_ticks;
}

/**
 * @brief 获取调度器运行时长
 * @return 调度器运行时长
 */
uint32_t scheduler_get_duration(void)
{
    return scheduler_duration;
}
