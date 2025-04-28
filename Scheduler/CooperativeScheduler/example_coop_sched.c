#include <stdio.h>
#include "coop_sched.h"

static int ticks = 0;
static int task1_index = -1;
static int task2_index = -1;
static int task3_index = -1;

// 错误报告函数
void error_report(uint32_t error_code)
{
    printf("Error reported: 0x%08X at %d ticks\n", error_code, ticks);
}

// 警告报告函数
void warning_report(uint32_t warning_code)
{
    printf("Warning reported: %d at %d ticks\n", warning_code, ticks);
}

// 模拟进入低功耗模式
void go_to_sleep(void)
{
    printf("System entering low-power mode at %d ticks\n", ticks);
}

void task1(void)
{
    printf("Task 1 is running at %d ticks\n", ticks);

    if (ticks == 10)
    {
        set_error_code(1); // 设置错误码
    }

    if (ticks == 16)
    {
        print_task_list("Task List");
        printf("Task count: %d\n", co_sch_task_count());
        if (co_sch_delete_task(task2_index) == task2_index)
        {
            printf("Successfully deleted Task 2 at %d ticks\n", ticks);
            print_task_list("Task List");
            printf("Task count: %d\n", co_sch_task_count());
        }
        else
        {
            printf("Failed to delete Task 2 at %d ticks\n", ticks);
        }
    }
}

void task2(void)
{
    printf("Task 2 is running at %d ticks\n", ticks);

    if (ticks == 12)
    {
        set_warning_code(10); // 设置警告码
    }
}

void task3(void)
{
    printf("Task 3 is running at %d ticks\n", ticks);
}

int main()
{
    // 设置错误报告函数
    set_error_report_func(error_report);

    // 设置警告报告函数
    set_warning_report_func(warning_report);

    // 设置低功耗模式函数
    set_go_to_sleep_func(go_to_sleep);

    // 创建任务
    task1_index = co_sch_create_task(task1, 1, 3); // 周期任务，延迟 1，周期 3
    task2_index = co_sch_create_task(task2, 2, 5); // 周期任务，延迟 2，周期 5
    task3_index = co_sch_create_task(task3, 0, 0); // 单次任务，立即执行

    if (task1_index < 0 || task2_index < 0 || task3_index < 0)
    {
        printf("Failed to create tasks\n");
        return -1;
    }

    // 启动调度器
    co_sch_start();

    while (1)
    {
        co_sch_update();
        co_sch_dispatch_tasks();
        ticks++;

        // 设置一个停止条件，实际使用时是不需要的
        if (ticks > 20)
        {
            // 停止调度器
            co_sch_stop();
            break;
        }
    }

    return 0;
}
