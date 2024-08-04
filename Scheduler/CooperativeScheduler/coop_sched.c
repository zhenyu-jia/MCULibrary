/**
 ******************************************************************************
 * @file    coop_sched.c
 * @author  JZY
 * @version V1.0.0
 * @date    2021-12-25
 * @brief   合作式调度器
 ******************************************************************************
 * @attention
 *
 * None
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "coop_sched.h"

/* 公用变量定义 --------------------------------------------------------------*/

/* 私有函数原型 --------------------------------------------------------------*/
static void cSch_report_status(void); // 用来显示错误代码的简单函数

/* 私有变量 ------------------------------------------------------------------*/
static cTask *task_head_handle = NULL;                // 任务链表的头指针
static cSch_go_to_sleep_func cSch_go_to_sleep = NULL; // 进入低功耗模式
static cSch_err_report_func cSch_err_report = NULL;   // 错误报告函数
static uint32_t err_code_mask = NO_ERROR_MASK;        // 错误码掩码
static uint8_t scheduler_running = 0;                 // 1: Running, 0: Stopped

/**
 * @brief  创建任务并返回任务句柄
 *
 * @param  pFunction  任务函数的指针
 * @param  delay      任务的初始延迟
 * @param  cycle      任务的周期
 *
 * @return  返回新创建任务的句柄，如果创建失败则返回 NULL
 */
cTask *cSch_create_task(void (*pFunction)(void), const uint16_t delay, const uint16_t cycle)
{
    // 初始化新的任务
    cTask *new_task = (cTask *)malloc(sizeof(cTask));
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
    cTask **current = &task_head_handle;
    while (*current != NULL)
    {
        current = &((*current)->next);
    }
    *current = new_task;

    return new_task; // 返回新任务的句柄
}

/**
 * @brief  通过任务句柄删除任务
 *
 * @param  task_handle  要删除的任务句柄
 *
 * @return  如果删除成功返回 0，失败返回 -1
 */
int cSch_delete_task(cTask *task_handle)
{
    // 检查任务链表是否为空
    if (task_head_handle == NULL || task_handle == NULL)
    {
        return -1; // 任务链表为空或任务句柄无效，删除失败
    }

    cTask **current = &task_head_handle;
    while (*current != NULL)
    {
        if (*current == task_handle)
        {
            cTask *to_delete = *current;
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
 * 该函数更新任务的延迟计数器，并设置任务的运行标志。
 * 应在 Systick 中断服务函数中调用。
 */
void cSch_updata(void)
{
    if (!scheduler_running)
    {
        return;
    }

    for (cTask *current = task_head_handle; current != NULL; current = current->next)
    {
        if (current->delay > 0)
        {
            current->delay--; // 任务还未准备好，延时减 1
        }
        else
        {
            current->runFlag++;
            if (current->cycle)
            {
                current->delay = current->cycle; // 调度定期的任务再次运行
            }
        }
    }
}

/**
 * @brief  调度函数
 *
 * 该函数运行所有需要执行的任务，并根据任务的周期和状态更新任务链表。
 * 应在主循环中调用。
 */
void cSch_dispatch_tasks(void)
{
    cTask **current = &task_head_handle;

    while (*current != NULL)
    {
        if ((*current)->runFlag > 0 && (*current)->pTask != NULL)
        {
            (*current)->pTask();   // 运行任务
            (*current)->runFlag--; // 复位/降低 runFlag 标志

            // 如果是单次任务，将它从列表中删除
            if ((*current)->cycle == 0)
            {
                cTask *to_delete = *current;
                *current = (*current)->next; // 更新链表指针
                free(to_delete);             // 释放已删除任务的内存
                continue;                    // 直接进入下一次循环，避免更新指针
            }
        }
        current = &((*current)->next); // 更新指向下一个任务的指针
    }

    // 报告系统状况
    cSch_report_status();
    // 调度器进入低功耗模式
    if (cSch_go_to_sleep != NULL)
    {
        cSch_go_to_sleep();
    }
}

/**
 * @brief  用来显示错误代码的简单函数
 *
 * 该函数用于报告系统的错误码。如果错误码有更改且不为无错误状态，则调用错误报告函数。
 */
static void cSch_report_status(void)
{
    static uint32_t pre_error_mask = NO_ERROR_MASK;

    if (err_code_mask != pre_error_mask && err_code_mask != NO_ERROR_MASK)
    {
        if (cSch_err_report != NULL)
        {
            cSch_err_report(err_code_mask);
        }
        pre_error_mask = err_code_mask;
    }
}

/**
 * @brief  计算任务数量
 *
 * @return  当前任务数量
 */
int cSch_task_count(void)
{
    int count = 0;
    for (cTask *current = task_head_handle; current != NULL; current = current->next)
    {
        count++;
    }
    return count;
}

/**
 * @brief  启动调度器
 *
 * 启动调度器并开始调度任务。如果任务链表为空或任务数为 0，则启动失败。
 */
void cSch_start(void)
{
    // 如果链表为空或任务数为0，启动失败
    if (task_head_handle == NULL || cSch_task_count() == 0)
    {
        return;
    }
    scheduler_running = 1;
}

/**
 * @brief  停止调度器
 *
 * 停止调度器并暂停任务调度。
 */
void cSch_stop(void)
{
    scheduler_running = 0;
}

/**
 * @brief  设置错误报告函数
 *
 * @param  func  错误报告函数的指针
 */
void set_err_report(cSch_err_report_func func)
{
    cSch_err_report = func;
}

/**
 * @brief  设置进入低功耗模式的函数
 *
 * @param  func  进入低功耗模式函数的指针
 */
void set_go_to_sleep(cSch_go_to_sleep_func func)
{
    cSch_go_to_sleep = func;
}

/**
 * @brief  设置错误码掩码
 *
 * @param  err_mask  错误码掩码
 */
void set_err_code_mask(uint32_t err_mask)
{
    err_code_mask = err_mask;
}

/**
 * @brief  获取错误码掩码
 *
 * @return  当前的错误码掩码
 */
uint32_t get_err_code_mask(void)
{
    return err_code_mask;
}

/**
 * @brief  打印任务列表
 *
 * @param  label  打印的标签
 */
void print_task_list(const char *label)
{
    printf("%s:\n", label);
    cTask *current = task_head_handle;
    while (current != NULL)
    {
        printf("Task at %p: delay=%u, cycle=%u, runFlag=%u, pTask=%p, next=%p\n",
               (void *)current, current->delay, current->cycle, current->runFlag,
               (void *)current->pTask, (void *)current->next);
        current = current->next;
    }
}