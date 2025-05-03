/**
 *******************************************************************************
 * @file    coop_sched.c
 * @author  Jia Zhenyu
 * @version V1.2.0
 * @date    2021-12-31
 * @brief   合作式调度器实现文件
 *
 * @details 本文件实现了合作式调度器的核心逻辑，适用于多任务环境下的任务切换。
 *          调度器通过轮询方式管理任务队列。
 *
 * @attention
 *          - 本文件为内部实现，不对外公开接口。
 *          - 使用时需确保所有任务函数均为非阻塞设计。
 *
 * @copyright
 *          Copyright (C) 2021 JZY. All rights reserved.
 *          This software is licensed under the MIT License.
 *
 * @history
 *          | Version | Date       | Author     | Changes                  |
 *          |---------|------------|------------|--------------------------|
 *          | V1.0.0  | 2021-12-25 | Jia Zhenyu | Initial creation         |
 *          | V1.1.0  | 2021-12-30 | Jia Zhenyu | Use linked lists         |
 *          | V1.2.0  | 2021-12-31 | Jia Zhenyu | Add event queue          |
 *******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "coop_sched.h"

/* 公用变量定义 --------------------------------------------------------------*/

/* 私有函数原型 --------------------------------------------------------------*/
static void co_sch_report_error(void);    // 用来报告错误
static void co_sch_report_warning(void);  // 用来报告警告
static void co_sch_sleep(void);           // 进入低功耗模式
static void co_sch_dispatch_tasks(void);  // 调度任务函数
static void co_sch_dispatch_events(void); // 调度事件函数

/* 私有变量 ------------------------------------------------------------------*/
static CO_TASK *co_sch_tasks_head_handle = NULL;          // 任务链表的头指针
static CO_EVENT co_event_queue[CO_SCH_MAX_EVENTS];            // 事件队列
static volatile uint8_t evt_front = 0;                    // 事件队列的前指针
static volatile uint8_t evt_rear = 0;                     // 事件队列的后指针
static co_sch_go_to_sleep_func co_sch_go_to_sleep = NULL; // 进入低功耗模式
static co_sch_report_func co_sch_report_err = NULL;       // 错误报告函数指针
static co_sch_report_func co_sch_report_warn = NULL;      // 警告报告函数指针
static uint32_t err_code_mask = NO_ERROR_MASK;            // 错误码掩码
static uint32_t warn_code = NO_WARNING;                   // 警告码
static uint8_t scheduler_running = 0;                     // 1: Running, 0: Stopped

/**
 * @brief  创建一个任务并返回任务句柄
 *
 * @param  pFunction 任务函数指针
 * @param  delay 任务首次执行前的延迟时间（范围：0 到 UINT16_MAX）
 * @param  cycle 任务循环执行的周期（范围：0 到 UINT16_MAX，0 表示单次任务）
 *
 * @retval 返回新创建任务的句柄，如果创建失败则返回 NULL
 */
CO_TASK *co_sch_create_task(const void (*pFunction)(void), const uint16_t delay, const uint16_t cycle)
{
    // 初始化新的任务
    CO_TASK *new_task = (CO_TASK *)malloc(sizeof(CO_TASK));
    if (new_task == NULL)
    {
        return NULL; // 内存分配失败
    }

    new_task->runFlag = 0;
    new_task->delay = delay;
    new_task->cycle = cycle;
    new_task->pTask = pFunction;
    new_task->next = NULL;

    // 将新任务添加到任务队列
    CO_TASK **current = &co_sch_tasks_head_handle;
    while (*current != NULL)
    {
        current = &((*current)->next);
    }
    *current = new_task;

    return new_task; // 返回新任务的句柄
}

/**
 * @brief  删除指定任务
 *
 * @param  task_handle 任务句柄，用于标识要删除的任务
 * @retval 如果删除成功返回 0，失败返回 -1
 */
int co_sch_delete_task(const CO_TASK *task_handle)
{
    // 检查任务链表是否为空
    if (co_sch_tasks_head_handle == NULL || task_handle == NULL)
    {
        return -1; // 任务链表为空或任务句柄无效，删除失败
    }

    CO_TASK **current = &co_sch_tasks_head_handle;
    while (*current != NULL)
    {
        if (*current == task_handle)
        {
            CO_TASK *to_delete = *current;
            *current = (*current)->next;
            free(to_delete);
            return 0;
        }
        current = &((*current)->next);
    }

    return -1; // 未找到任务，删除失败
}

/**
 * @brief  调度器更新函数
 *
 * 此函数用于更新任务调度器的状态，通常在 Systick 定时器中断服务函数中调用。
 * 它会遍历任务队列，检查每个任务的延迟时间是否已到期。如果延迟时间为 0，
 * 则将任务标记为准备运行，并根据任务的周期性设置下次运行的延迟时间。
 *
 * @note 如果任务是单次任务（周期为 0），它只会被标记为运行一次。
 *
 * @param  None
 * @retval None
 */
void co_sch_update(void)
{
    if (!scheduler_running)
    {
        return;
    }

    for (CO_TASK *current = co_sch_tasks_head_handle; current != NULL; current = current->next)
    {
        if (current->delay > 0)
        {
            current->delay--; // 任务还未准备好，延时减 1
        }
        else
        {
            if (current->runFlag < UINT16_MAX)
            {
                current->runFlag++;
            }

            if (current->cycle)
            {
                // 调度定期的任务再次运行
                current->delay = current->cycle - 1; // 由于这行代码也会占用一个周期，所以这里要减 1
            }
        }
    }
}

/**
 * @brief  调度任务函数
 *
 * @param  None
 * @retval None
 */
static void co_sch_dispatch_tasks(void)
{
    CO_TASK **current = &co_sch_tasks_head_handle;

    while (*current != NULL)
    {
        if ((*current)->runFlag > 0 && (*current)->pTask != NULL)
        {
            (*current)->pTask();   // 运行任务
            (*current)->runFlag--; // 复位/降低 runFlag 标志

            // 如果是单次任务，将它从列表中删除
            if ((*current)->cycle == 0)
            {
                CO_TASK *to_delete = *current;
                *current = (*current)->next; // 更新链表指针
                free(to_delete);             // 释放已删除任务的内存
                continue;                    // 直接进入下一次循环，避免更新指针
            }
        }
        current = &((*current)->next); // 更新指向下一个任务的指针
    }

#ifdef CO_SCH_REPORT_ERRORS // 报告错误
    co_sch_report_error();
#endif /* CO_SCH_REPORT_ERRORS */

#ifdef CO_SCH_REPORT_WARNINGS // 报告警告
    co_sch_report_warning();
#endif /* CO_SCH_REPORT_WARNINGS */

#ifdef CO_SCH_GO_TO_SLEEP // 进入低功耗模式
    co_sch_sleep();
#endif /* CO_SCH_GO_TO_SLEEP */
}

/**
 * @brief  发布事件到事件队列
 *
 * @param  pFunction 事件处理函数指针
 * @param  arg  传递给事件处理函数的参数
 * @retval 0: 成功，-1: 失败（队列已满）
 */
int co_sch_post_event(const void (*pFunction)(void *), void *arg)
{
    uint8_t next = (evt_rear + 1) % CO_SCH_MAX_EVENTS;

    if (next == evt_front)
    {
        return -1; // 队列已满，无法添加事件
    }
    co_event_queue[evt_rear].pEvent = pFunction;
    co_event_queue[evt_rear].arg = arg;
    evt_rear = next;
    return 0;
}

/**
 * @brief  发布事件到事件队列（中断安全版本）
 *
 * @param  pFunction 事件处理函数指针
 * @param  arg  传递给事件处理函数的参数
 * @retval 0: 成功，-1: 失败（队列为空）
 */
int co_sch_post_event_from_isr(const void (*pFunction)(void *), void *arg)
{
    return co_sch_post_event(pFunction, arg); // 简化处理（无锁）；可根据平台添加禁中断保护
}

/**
 * @brief  调度事件函数
 *
 * @param  None
 * @retval None
 */
static void co_sch_dispatch_events(void)
{
    // 处理事件队列
    while (evt_front != evt_rear)
    {
        CO_EVENT evt = co_event_queue[evt_front];
        evt_front = (evt_front + 1) % CO_SCH_MAX_EVENTS;

        if (evt.pEvent != NULL)
        {
            evt.pEvent(evt.arg);
        }
    }
}

/**
 * @brief  调度器运行函数
 *
 * @note   该函数在主循环中被调用，负责调度任务和事件。
 *         如果调度器未运行，则不会执行任何操作。
 *
 * @param  None
 * @retval None
 */
void co_sch_run(void)
{
    // 运行调度器
    if (scheduler_running)
    {
        co_sch_dispatch_tasks();  // 调度任务
        co_sch_dispatch_events(); // 调度事件
    }
}

/**
 * @brief  启动调度器
 *
 * @param  None
 * @retval None
 */
void co_sch_start(void)
{
    // 如果链表为空或任务数为 0，启动失败
    if (co_sch_tasks_head_handle == NULL || co_sch_task_count() <= 0)
    {
        return;
    }
    scheduler_running = 1;
}

/**
 * @brief  停止调度器
 *
 * @param  None
 * @retval None
 */
void co_sch_stop(void)
{
    scheduler_running = 0;
}

/**
 * @brief  用来显示错误代码
 *
 * 可以用 LED、蜂鸣器、或者串口打印等
 *
 * @param  None
 * @retval None
 */
static void co_sch_report_error(void)
{
    static uint32_t pre_err_mask = NO_ERROR_MASK;

    if (err_code_mask != pre_err_mask && err_code_mask != NO_ERROR_MASK)
    {
        if (co_sch_report_err != NULL)
        {
            co_sch_report_err(err_code_mask);
        }
        pre_err_mask = err_code_mask;
    }
}

/**
 * @brief  用来显示警告代码
 *
 * 可以用 LED、蜂鸣器、或者串口打印等
 *
 * @param  None
 * @retval None
 */
static void co_sch_report_warning(void)
{
    static uint32_t pre_warn_code = NO_WARNING;
    static uint32_t warn_ticks_count = 0;

    if (warn_code != pre_warn_code)
    {
        if (co_sch_report_warn != NULL)
        {
            co_sch_report_warn(warn_code);
        }
        pre_warn_code = warn_code;

        if (warn_code != NO_WARNING)
        {
            warn_ticks_count = CO_SCH_REPORT_WARNINGS_TICKS - 1; // 由于这行代码也会占用一个周期，所以这里要减 1

            if (warn_ticks_count == 0)
            {
                warn_code = NO_WARNING;
            }
        }
        else
        {
            warn_ticks_count = 0;
        }
    }
    else
    {
        if (warn_ticks_count != 0)
        {
            if (--warn_ticks_count == 0)
            {
                warn_code = NO_WARNING;
            }
        }
    }
}

/**
 * @brief  系统调度睡眠函数
 *
 * @param  None
 * @retval None
 */
static void co_sch_sleep(void)
{
    if (co_sch_go_to_sleep != NULL)
    {
        co_sch_go_to_sleep();
    }
}

/**
 * @brief  计算任务数量
 *
 * @return  当前任务数量
 */
int co_sch_task_count(void)
{
    int count = 0;
    for (CO_TASK *current = co_sch_tasks_head_handle; current != NULL; current = current->next)
    {
        count++;
    }
    return count;
}

/**
 * @brief  设置错误报告函数
 *
 * @param  func 错误报告函数的指针
 * @retval None
 */
void set_error_report_func(const co_sch_report_func func)
{
    co_sch_report_err = func;
}

/**
 * @brief  设置警告报告函数
 *
 * @param  func 警告报告函数的指针
 * @retval None
 */
void set_warning_report_func(const co_sch_report_func func)
{
    co_sch_report_warn = func;
}

/**
 * @brief  设置进入低功耗模式的函数
 *
 * @param  func 进入低功耗模式函数的指针
 * @retval None
 */
void set_go_to_sleep_func(const co_sch_go_to_sleep_func func)
{
    co_sch_go_to_sleep = func;
}

/**
 * @brief  设置错误码
 *
 * @note   该函数将错误码掩码的第 err_count 位设置为 1，表示该错误发生
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval None
 */
void set_error_code(const uint8_t err_count)
{
    if (err_count < 32)
    {
        err_code_mask |= (1U << err_count); // 将第 err_count 位设置为 1
    }
}

/**
 * @brief  复位错误码
 *
 * @note   该函数将错误码掩码的第 err_count 位清除为 0，表示该错误已被处理
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval None
 */
void reset_error_code(const uint8_t err_count)
{
    if (err_count < 32)
    {
        err_code_mask &= ~(1U << err_count); // 清除第 err_count 位
    }
}

/**
 * @brief  获取错误码
 *
 * @param  err_count 错误码索引（0 到 31）
 * @retval 当前的错误码
 */
uint8_t get_error_code(const uint8_t err_count)
{
    if (err_count < 32)
    {
        return (err_code_mask & (1U << err_count)) ? 1 : 0; // 如果对应位为 1，表示错误发生
    }

    return 0; // 如果错误码索引无效，返回 0
}

/**
 * @brief  获取所有错误码
 *
 * @param  None
 * @retval 当前的错误码掩码
 */
uint32_t get_error_code_mask(void)
{
    return err_code_mask;
}

/**
 * @brief  设置警告码
 *
 * @param  warn  警告码
 * @retval None
 */
void set_warning_code(const uint32_t warn)
{
    warn_code = warn;
}

/**
 * @brief  获取警告码
 *
 * @param  None
 * @retval 当前的警告码
 */
uint32_t get_warning_code(void)
{
    return warn_code;
}

/**
 * @brief  打印任务列表
 *
 * @param  label 打印的标签
 * @retval None
 */
void print_task_list(const char *label)
{
    printf("%s:\n", label);
    CO_TASK *current = co_sch_tasks_head_handle;
    while (current != NULL)
    {
        printf("Task at %p: delay=%u, cycle=%u, runFlag=%u, pTask=%p, next=%p\n",
               (void *)current, current->delay, current->cycle, current->runFlag,
               (void *)current->pTask, (void *)current->next);
        current = current->next;
    }
}
