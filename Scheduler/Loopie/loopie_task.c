/**
 * @file    loopie_task.c
 * @brief   任务管理模块，实现任务的创建、删除、挂起、恢复、调度等功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#include "loopie_task.h"
#include "loopie_config.h"
#include "loopie_scheduler.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static TASK *task_list = NULL;             // 任务链表
static volatile int task_count = 0;        // 当前任务数量
extern uint32_t (*get_system_ticks)(void); // 获取系统时间

/**
 * @brief 初始化
 */
void task_init(void)
{
    task_list = NULL;
    task_count = 0;
}

/**
 * @brief 创建任务
 * @param pTask 任务函数
 * @param arg 任务参数
 * @param priority 任务优先级
 * @param delay 任务延迟
 * @param cycle 任务周期（为 0 时表示为单次任务）
 * @retval 任务句柄
 */
TASK *task_create(void (*const pTask)(void *), void *const arg, const int8_t priority, const uint16_t delay, const uint16_t cycle)
{
    if (!pTask || priority > SCH_USER_PRIORITY_MAX || priority < SCH_USER_PRIORITY_MIN)
    {
        return NULL; // 参数错误
    }

    // 检查是否已达到最大任务数限制
    if (task_count >= SCH_MAX_TASK_NUM)
    {
        return NULL; // 达到最大任务数，不能创建新任务
    }

    TASK *new_task = (TASK *)malloc(sizeof(TASK));
    if (!new_task)
    {
        return NULL; // 内存分配失败
    }

    memset(new_task, 0, sizeof(TASK));
    new_task->pTask = pTask;
    new_task->arg = arg;
    new_task->priority = priority;
    new_task->delay = delay;
    new_task->cycle = cycle;

    // 按优先级插入
    uint32_t primask = enter_critical();

    TASK **current_task = &task_list;
    while (*current_task && (*current_task)->priority <= new_task->priority)
    {
        current_task = &(*current_task)->next;
    }
    new_task->next = *current_task;
    *current_task = new_task;

    exit_critical(primask);
    task_count++; // 任务数量增加
    return new_task;
}

/**
 * @brief 删除任务
 * @param task_handle 任务句柄
 * @retval -1 任务句柄不能为空或任务列表为空，-2 任务句柄不存在，0 删除成功
 */
int task_delete(TASK *task_handle)
{
    if (!task_handle || !task_list)
    {
        return -1; // 任务句柄不能为空或任务列表为空
    }

    uint32_t primask = enter_critical();

    TASK **current_task = &task_list;
    while (*current_task)
    {
        if (*current_task == task_handle)
        {
            *current_task = task_handle->next;
            free(task_handle);
            task_handle = NULL; // 清空指针，避免悬空指针
            task_count--;       // 删除任务时减少任务数量
            exit_critical(primask);
            return 0; // 删除成功
        }
        current_task = &(*current_task)->next;
    }

    exit_critical(primask);
    return -2; // 未找到
}

/**
 * @brief 安全地删除任务，并将任务句柄置为 NULL
 * @param task_handle 任务句柄的指针（注意是指针的指针）
 * @retval -1 任务句柄不能为空或任务列表为空，-2 任务句柄不存在，0 删除成功
 */
int task_delete_safe(TASK **task_handle)
{
    if (!task_handle || !*task_handle || !task_list)
    {
        return -1; // 参数错误或任务列表为空
    }

    uint32_t primask = enter_critical();

    TASK **current_task = &task_list;
    while (*current_task)
    {
        if (*current_task == *task_handle)
        {
            *current_task = (*task_handle)->next;
            free(*task_handle);
            *task_handle = NULL; // 清空调用者的指针
            task_count--;
            exit_critical(primask);
            return 0; // 删除成功
        }
        current_task = &(*current_task)->next;
    }

    exit_critical(primask);
    return -2; // 未找到
}

/**
 * @brief 挂起任务
 * @param task_handle 任务句柄
 * @retval 0 成功，-1 失败
 */
int task_suspend(TASK *task_handle)
{
    if (!task_handle)
    {
        return -1; // 任务句柄不能为空
    }

    task_handle->suspendFlag = 1;

    return 0;
}

/**
 * @brief 恢复任务
 * @param task_handle 任务句柄
 * @retval 0 成功，-1 失败
 */
int task_resume(TASK *task_handle)
{
    if (!task_handle)
    {
        return -1; // 任务句柄不能为空
    }

    task_handle->suspendFlag = 0;

    return 0;
}

/**
 * @brief 更新任务状态
 */
void task_update(void)
{
    TASK *current_task = task_list;

    while (current_task)
    {
        if (!current_task->suspendFlag)
        {
            if (current_task->delay > 0)
            {
                current_task->delay--;
            }
            else
            {
                if (current_task->runFlag < SCH_MAX_TASK_RUN_FLAG)
                {
                    current_task->runFlag++;
                }

                if (current_task->cycle)
                {
                    current_task->delay = current_task->cycle - 1; // 由于这行代码也会占用一个周期，所以这里要减 1
                }
            }
        }
        current_task = current_task->next;
    }
}

/**
 * @brief 运行任务
 */
void task_run(void)
{
    TASK **current_task = &task_list;

    while (*current_task)
    {
        if (!(*current_task)->suspendFlag && (*current_task)->runFlag > 0 && (*current_task)->pTask)
        {
            // 记录当前时间
            uint32_t now = get_system_ticks ? get_system_ticks() : 0;
            (*current_task)->interval = now - (*current_task)->last_run_time;
            (*current_task)->last_run_time = now;

            // 执行任务
            (*current_task)->pTask((void *)(*current_task)->arg);
            (*current_task)->runFlag--;

            // 判断是否需要删除一次性任务
            if ((*current_task)->cycle == 0)
            {
                TASK *temp = *current_task;
                *current_task = (*current_task)->next; // 删除当前任务
                free(temp);
                task_count--; // 删除任务时减少任务数量
                continue;     // 继续下一个任务
            }
        }

        current_task = &(*current_task)->next;
    }
}

/**
 * @brief 获取任务数量
 * @retval 任务数量
 */
int task_get_count(void)
{
    return task_count;
}

/**
 * @brief 获取任务运行间隔
 * @param task_handle 任务句柄
 * @retval 任务运行间隔，单位为毫秒
 */
uint32_t task_get_interval(TASK *task_handle)
{
    if (!task_handle)
    {
        return 0;
    }

    return task_handle->interval;
}
