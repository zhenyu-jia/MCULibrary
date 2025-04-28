/**
 *******************************************************************************
 * @file    coop_sched.c
 * @author  Jia Zhenyu
 * @version V1.0.0
 * @date    2021-12-25
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
 *******************************************************************************
 */

/* Includes -----------------------------------------------------------------*/
#include <stdio.h>
#include "coop_sched.h"

/* 公用变量定义 --------------------------------------------------------------*/

/* 私有函数原型 --------------------------------------------------------------*/
static void co_sch_report_error(void);   // 用来报告错误
static void co_sch_report_warning(void); // 用来报告警告
static void co_sch_sleep(void);          // 进入低功耗模式

/* 私有变量 ------------------------------------------------------------------*/
static CO_TASK co_sch_tasks[CO_SCH_MAX_TASKS] = {{0, 0, 0, NULL}}; // 任务队列
static co_sch_go_to_sleep_func co_sch_go_to_sleep = NULL;          // 进入低功耗模式
static co_sch_report_func co_sch_report_err = NULL;                // 错误报告函数指针
static co_sch_report_func co_sch_report_warn = NULL;               // 警告报告函数指针
static uint32_t err_code_mask = NO_ERROR_MASK;                     // 错误码掩码
static uint32_t warn_code = NO_WARNING;                            // 警告码
static uint8_t scheduler_running = 0;                              // 1: Running, 0: Stopped

/**
 * @brief  创建一个任务
 *
 * @param  pFunction 任务函数指针
 * @param  delay 任务首次执行前的延迟时间（范围：0 到 UINT16_MAX）
 * @param  cycle 任务循环执行的周期（范围：0 到 UINT16_MAX，0 表示单次任务）
 *
 * @retval 如果函数指针无效或任务队列已满，则返回相应的错误码；否则返回任务在队列中的索引
 */
int co_sch_create_task(const void (*pFunction)(void), const uint16_t delay, const uint16_t cycle)
{
    int index = 0;

    /* 检查任务函数指针是否有效 */
    if (pFunction == NULL)
    {
        return ERROR_INVALID_PARAM; // 无效的函数指针
    }

    /* 首先在任务队列中找到一个空隙（如果有的话） */
    while ((index < CO_SCH_MAX_TASKS) && (co_sch_tasks[index].pTask != NULL))
    {
        index++;
    }

    /* 是否已经到达队列的结尾？ */
    if (index >= CO_SCH_MAX_TASKS)
    {
        return ERROR_TASK_QUEUE_FULL; // 任务队列已满
    }

    /* 初始化任务 */
    co_sch_tasks[index].runFlag = 0;
    co_sch_tasks[index].delay = delay;
    co_sch_tasks[index].cycle = cycle;
    co_sch_tasks[index].pTask = pFunction;

    return index; // 返回任务索引，以便以后删除
}

/**
 * @brief  删除指定任务
 *
 * @param  task_index 任务索引，用于标识要删除的任务
 * @retval 如果任务成功删除，返回删除任务的任务索引；
 *         如果任务索引无效，返回 ERROR_INVALID_PARAM；
 *         如果任务未找到，返回 ERROR_TASK_NOT_FOUND。
 */
int co_sch_delete_task(const int task_index)
{
    if (task_index < 0 || task_index >= CO_SCH_MAX_TASKS)
    {
        return ERROR_INVALID_PARAM; // 任务索引无效
    }

    if (co_sch_tasks[task_index].pTask == NULL)
    {
        return ERROR_TASK_NOT_FOUND; // 任务未找到（无法删除）
    }

    /* 删除任务 */
    co_sch_tasks[task_index].pTask = NULL;
    co_sch_tasks[task_index].delay = 0;
    co_sch_tasks[task_index].cycle = 0;
    co_sch_tasks[task_index].runFlag = 0;

    return task_index; // 返回任务索引
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

    for (int index = 0; index < CO_SCH_MAX_TASKS; index++)
    {
        /* 检查任务是否已经准备好运行 */
        if (co_sch_tasks[index].pTask != NULL)
        {
            if (co_sch_tasks[index].delay > 0)
            {
                co_sch_tasks[index].delay--; // 任务还未准备好，延时减 1
            }
            else
            {
                if (co_sch_tasks[index].runFlag < UINT16_MAX)
                {
                    co_sch_tasks[index].runFlag++; // "runFlag" 标志加 1
                }

                if (co_sch_tasks[index].cycle > 0)
                {
                    /* 调度周期的任务再次运行 */
                    co_sch_tasks[index].delay = co_sch_tasks[index].cycle - 1; // 由于这行代码也会占用一个周期，所以这里要减 1
                }
            }
        }
    }
}

/**
 * @brief  调度函数
 *
 * 当一个任务（函数）需要运行时，co_sch_dispatch_tasks() 将运行它
 * 该函数在主循环中被调用
 *
 * @param  None
 * @retval None
 */
void co_sch_dispatch_tasks(void)
{
    int index;

    if (!scheduler_running)
    {
        return;
    }

    /* 调度（运行）下一个任务（如果有任务就绪） */
    for (index = 0; index < CO_SCH_MAX_TASKS; index++)
    {
        if (co_sch_tasks[index].runFlag > 0 && co_sch_tasks[index].pTask != NULL)
        {
            (*co_sch_tasks[index].pTask)(); // 运行任务
            co_sch_tasks[index].runFlag--;  // 复位/降低 runFlag 标志

            /* 如果是单次任务，将它从列表中删除 */
            if (co_sch_tasks[index].cycle == 0)
            {
                co_sch_delete_task(index);
            }
        }
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
 * @brief  启动调度器
 *
 * @param  None
 * @retval None
 */
void co_sch_start(void)
{
    /* 如果任务数为 0，启动失败 */
    if (co_sch_task_count() <= 0)
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
 * @param  None
 * @retval 当前任务数量
 */
int co_sch_task_count(void)
{
    int index = 0;
    int count = 0;

    while (index < CO_SCH_MAX_TASKS)
    {
        if (co_sch_tasks[index].pTask != NULL)
        {
            count++;
        }

        index++;
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
    int index = 0;

    printf("%s:\n", label);
    while (index < CO_SCH_MAX_TASKS)
    {
        if (co_sch_tasks[index].pTask != NULL)
        {
            printf("Task at %d: delay=%u, cycle=%u, runFlag=%u, pTask=%p\n", index,
                   co_sch_tasks[index].delay, co_sch_tasks[index].cycle, co_sch_tasks[index].runFlag,
                   (void *)co_sch_tasks[index].pTask);
        }

        index++;
    }
}
