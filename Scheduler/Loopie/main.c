#include <stdio.h>
#include "loopie_scheduler.h"

uint32_t ticks = 0;

void task1(void *arg)
{
    printf("Task 1 is running, arg: %s, at ticks: %d\n", (char *)arg, ticks);
}

void task2(void *arg)
{
    printf("Task 2 is running, arg: %s, at ticks: %d\n", (char *)arg, ticks);
}

void task3(void *arg)
{
    printf("Task 3 is running, arg: %s, at ticks: %d\n", (char *)arg, ticks);
}

// 事件任务
void buttonHandler1(void *arg)
{
    printf("[事件 1] 按钮被按下！参数 = %d, at ticks: %d\n", *(int *)arg, ticks);
}
void buttonHandler2(void *arg)
{
    printf("[事件 2] 按钮被按下！参数 = %d, at ticks: %d\n", *(int *)arg, ticks);
}
void buttonHandler3(void *arg)
{
    printf("[事件 3] 按钮被按下！参数 = %d, at ticks: %d\n", arg ? *(int *)arg : 0, ticks);
}

// 事件标志处理函数
void my_event_handler(uint32_t flag_count)
{
    printf("Flag %u triggered!\n", flag_count);
}

// 空闲任务
void my_idle_handler(void)
{
    printf("Idle task running!\n");
}

// 获取系统时间函数
uint32_t my_get_system_ticks(void)
{
    return ticks;
}

// 错误报告函数
void my_error_report(uint32_t error_code)
{
    printf("Error reported: 0x%08X at %d ticks\n", error_code, ticks);
}

// 警告报告函数
void my_warning_report(uint32_t warning_code)
{
    printf("Warning reported: %d at %d ticks\n", warning_code, ticks);
}

int main()
{
    // 设置错误报告函数
    error_set_report_func(my_error_report);

    // 设置警告报告函数
    warning_set_report_func(my_warning_report);

    // 设置事件标志处理函数
    event_flag_cb_set_callback(my_event_handler);

    // 设置空闲任务
    scheduler_set_idle_hook(my_idle_handler);

    // 设置获取系统时间函数
    scheduler_set_time_func(my_get_system_ticks);

    scheduler_init();

    TASK *task1_handle = task_create(task1, (char *)"task1", 127, 1, 1);
    if (task1_handle == NULL)
    {
        printf("task1 create failed\n");
        return -1;
    }
    TASK *task2_handle = task_create(task2, (char *)"task2", 126, 1, 1);
    if (task2_handle == NULL)
    {
        printf("task2 create failed\n");
        return -1;
    }

    TASK *task3_handle = task_create(task3, (char *)"task3", 125, 1, 1);
    if (task3_handle == NULL)
    {
        printf("task3 create failed\n");
        return -1;
    }

    scheduler_start();

    while (1)
    {
        scheduler_update();
        scheduler_run();

        if (ticks == 5)
        {
            task_suspend(task1_handle);

            error_code_set(0);
            error_code_set(1);
            warning_code_set(1);
            warning_code_set(2); // 警告会覆盖一个周期内的
        }
        if (ticks == 6)
        {
            event_post(buttonHandler1, &ticks, EVENT_POST_DISCARD); // 模拟按钮事件
        }
        if (ticks == 7)
        {
            event_post(buttonHandler2, &ticks, EVENT_POST_DISCARD); // 模拟按钮事件

            if (event_flag_set(5) == -1)
            {
                printf("event flag 3 set fail\n");
            }
        }
        if (ticks == 8)
        {
            if (event_flag_cb_set(3) == -1)
            {
                printf("event flag 3 set fail\n");
            }

            if (event_flag_cb_set(7) == -1)
            {
                printf("event flag 7 set fail\n");
            }
            event_post_default(buttonHandler3); // 模拟按钮事件

            error_code_clear(0);
            warning_code_set(0);
        }
        if (ticks == 9)
        {
            if (event_flag_get(5))
            {
                event_flag_clear(5);
                printf("Flag 5 triggered!\n");
            }
        }
        if (ticks == 11)
        {
            int count_buf[SCH_MAX_EVENT_NUM];
            for (int count = 0; count < SCH_MAX_EVENT_NUM; count++)
            {
                count_buf[count] = count;
                event_post(buttonHandler3, &count_buf[count], EVENT_POST_DISCARD);
                printf("event queue free size: %d\n", event_queue_free_size());
            }
        }
        if (ticks == 12)
        {
            printf("event queue free size: %d\n", event_queue_free_size());
            task_resume(task1_handle);
        }
        if (ticks == 15)
        {
            task_delete(task1_handle);
        }
        if (ticks == 16)
        {
            if (task3_handle == NULL)
            {
                printf("task3 create failed\n");
            }
            else
            {
                printf("task3 created\n");
            }
        }
        if (ticks == 20)
        {
            // task_delete_safe(&task3_handle);
            printf("task count: %d\n", task_get_count());
        }
        if (ticks == 21)
        {
            printf("task count: %d\n", task_get_count());
            if (task3_handle == NULL)
            {
                printf("task3 deleted\n");
            }
            else
            {
                printf("task3 not deleted\n");
            }
        }
        if (ticks == 25)
        {
            break;
        }
        ticks++;
    }

    return 0;
}
