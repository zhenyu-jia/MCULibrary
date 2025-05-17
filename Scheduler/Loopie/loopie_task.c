/**
 * @file    loopie_task.c
 * @brief   任务管理模块，实现任务的创建、删除、挂起、恢复、调度等功能
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.1.0
 */

#include "loopie_task.h"
#include "loopie_config.h"
#include "loopie_critical.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static TASK task_array[SCH_TASK_MAX_NUM] = {0}; // 任务数组
static volatile int task_count = 0;             // 当前任务数量
extern uint32_t (*get_system_ticks)(void);      // 获取系统时间

/**
 * @brief 初始化
 * @note 该函数目前不调用也没关系，只是为了程序结构完整方便后续扩展
 */
void task_init(void)
{
    memset(task_array, 0, sizeof(task_array));
    task_count = 0;
}

/**
 * @brief  创建任务
 * @param pTask 任务函数
 * @param arg 任务参数
 * @param delay 任务延迟
 * @param cycle 任务周期（为 0 时表示为单次任务）
 * @retval 如果函数指针无效或任务队列已满，则返回相应的错误码；否则返回任务在队列中的索引
 */
int task_create(void (*const pTask)(void *), void *const arg, const uint16_t delay, const uint16_t cycle)
{
    /* 检查任务函数指针是否有效 */
    if (pTask == NULL)
    {
        return ERROR_TASK_INVALID_PARAM; // 无效的函数指针
    }

    uint32_t primask = enter_critical();

    int index = 0;

    /* 首先在任务队列中找到一个空隙（如果有的话） */
    while ((index < SCH_TASK_MAX_NUM) && (task_array[index].pTask != NULL))
    {
        index++;
    }

    /* 是否已经到达队列的结尾？ */
    if (index >= SCH_TASK_MAX_NUM)
    {
        return ERROR_TASK_QUEUE_FULL; // 任务队列已满
    }

    /* 初始化任务 */
    task_array[index].pTask = pTask;
    task_array[index].arg = arg;
    task_array[index].runFlag = 0;
    task_array[index].suspendFlag = 0;
    task_array[index].delay = delay;
    task_array[index].cycle = cycle;
    task_array[index].last_run_time = 0;
    task_array[index].interval = 0;

    task_count++;
    exit_critical(primask);
    return index; // 返回任务索引，以便以后删除
}

/**
 * @brief  删除任务
 * @param  task_index 任务索引，用于标识要删除的任务
 * @retval 如果任务成功删除，返回删除任务的任务索引；
 *         如果任务索引无效，返回 ERROR_TASK_INVALID_PARAM；
 *         如果任务未找到，返回 ERROR_TASK_NOT_FOUND。
 */
int task_delete(const int task_index)
{
    if (task_index < 0 || task_index >= SCH_TASK_MAX_NUM)
    {
        return ERROR_TASK_INVALID_PARAM; // 任务索引无效
    }

    uint32_t primask = enter_critical();

    if (task_array[task_index].pTask == NULL)
    {
        exit_critical(primask);
        return ERROR_TASK_NOT_FOUND; // 任务未找到（无法删除）
    }

    /* 删除任务 */
    task_array[task_index].pTask = NULL;
    task_array[task_index].arg = 0;
    task_array[task_index].runFlag = 0;
    task_array[task_index].suspendFlag = 0;
    task_array[task_index].delay = 0;
    task_array[task_index].cycle = 0;
    task_array[task_index].last_run_time = 0;
    task_array[task_index].interval = 0;

    task_count--;
    exit_critical(primask);
    return task_index;
}

/**
 * @brief 挂起任务
 * @param task_index 任务索引
 * @retval 0 成功 ERROR_TASK_INVALID_PARAM 失败
 */
int task_suspend(const int task_index)
{
    if (task_index < 0 || task_index >= SCH_TASK_MAX_NUM)
    {
        return ERROR_TASK_INVALID_PARAM; // 任务索引无效
    }

    task_array[task_index].suspendFlag = 1;

    return 0;
}

/**
 * @brief 恢复任务
 * @param task_index 任务索引
 * @retval 0 成功 ERROR_TASK_INVALID_PARAM 失败
 */
int task_resume(const int task_index)
{
    if (task_index < 0 || task_index >= SCH_TASK_MAX_NUM)
    {
        return ERROR_TASK_INVALID_PARAM; // 任务索引无效
    }

    task_array[task_index].suspendFlag = 0;

    return 0;
}

/**
 * @brief 更新任务状态
 */
void task_update(void)
{
    for (int index = 0; index < SCH_TASK_MAX_NUM; index++)
    {
        /* 检查任务是否已经准备好运行 */
        if (!task_array[index].suspendFlag && task_array[index].pTask)
        {
            if (task_array[index].delay > 0)
            {
                task_array[index].delay--; // 任务还未准备好，延时减 1
            }
            else
            {
                if (task_array[index].runFlag < SCH_TASK_MAX_RUN_FLAG) // 防止运行标志溢出
                {
                    task_array[index].runFlag++; // "runFlag" 标志加 1
                }

                if (task_array[index].cycle > 0)
                {
                    /* 调度周期的任务再次运行 */
                    task_array[index].delay = task_array[index].cycle - 1; // 由于这行代码也会占用一个周期，所以这里要减 1
                }
            }
        }
    }
}

/**
 * @brief 运行任务
 */
void task_run(void)
{
    for (int index = 0; index < SCH_TASK_MAX_NUM; index++)
    {
        if (!task_array[index].suspendFlag && task_array[index].runFlag > 0 && task_array[index].pTask)
        {
            // 记录当前时间
            uint32_t now = get_system_ticks ? get_system_ticks() : 0;
            task_array[index].interval = now - task_array[index].last_run_time;
            task_array[index].last_run_time = now;

            // 执行任务
            (*task_array[index].pTask)((void *)task_array[index].arg); // 运行任务
            task_array[index].runFlag--;                               // 复位/降低 runFlag 标志

            /* 如果是单次任务，将它从列表中删除 */
            if (task_array[index].cycle == 0)
            {
                task_delete(index);
            }
        }
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
 * @param task_index 任务索引
 * @retval 任务运行间隔
 */
uint32_t task_get_interval(const int task_index)
{
    if (task_index < 0 || task_index >= SCH_TASK_MAX_NUM)
    {
        return 0; // 任务索引无效返回 0
    }

    return task_array[task_index].interval;
}
