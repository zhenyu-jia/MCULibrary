// 测试 1、基本功能测试
// #include <stdio.h>
// #include "hyb_sched.h"

// void task1(void)
// {
//     printf("Task 1 is running\n");
// }

// void task2(void)
// {
//     printf("Task 2 is running\n");
// }

// int main(void)
// {
//     HYB_TASK *task1_handle = hyb_sch_create_task(task1, 1, 0, 0); // 单次任务，延迟 1
//     HYB_TASK *task2_handle = hyb_sch_create_task(task2, 2, 5, 1); // 周期任务，延迟 2，周期 5

//     hyb_sch_start();

//     for (int i = 0; i < 10; i++)
//     {
//         printf("\nTick %d\n", i);
//         hyb_sch_update();
//         hyb_sch_dispatch_tasks();
//     }

//     hyb_sch_stop();

//     return 0;
// }

// 测试 2、任务删除测试
// #include <stdio.h>
// #include "hyb_sched.h"

// void task1(void)
// {
//     printf("Task 1 is running\n");
// }

// void task2(void)
// {
//     printf("Task 2 is running\n");
// }

// int main(void)
// {
//     HYB_TASK *task1_handle = hyb_sch_create_task(task1, 1, 1, 0); // 单次任务
//     HYB_TASK *task2_handle = hyb_sch_create_task(task2, 2, 5, 1); // 周期任务

//     hyb_sch_start();

//     for (int i = 0; i < 10; i++)
//     {
//         printf("\nTick %d\n", i);
//         hyb_sch_update();
//         hyb_sch_dispatch_tasks();

//         if (i == 5)
//         { // 在第 5 个 tick 时删除任务 1
//             if (hyb_sch_delete_task(task1_handle) == 0)
//             {
//                 printf("Successfully deleted task 1\n");
//             }
//             else
//             {
//                 printf("Failed to delete task 1\n");
//             }
//         }
//     }

//     hyb_sch_stop();

//     return 0;
// }

// 测试 3、低功耗模式测试
// #include <stdio.h>
// #include "hyb_sched.h"

// void task1(void)
// {
//     printf("Task 1 is running\n");
// }

// void go_to_sleep(void)
// {
//     printf("System is going to sleep\n");
// }

// int main(void)
// {
//     set_go_to_sleep_func(go_to_sleep);

//     hyb_sch_create_task(task1, 1, 0, 1);
//     hyb_sch_start();

//     for (int i = 0; i < 5; i++)
//     {
//         printf("\nTick %d\n", i);
//         hyb_sch_update();
//         hyb_sch_dispatch_tasks();
//     }

//     hyb_sch_stop();

//     return 0;
// }

// 测试 4、边界条件测试
// #include <stdio.h>
// #include "hyb_sched.h"

// // 全局错误报告函数
// void error_report(uint32_t err)
// {
//     printf("Error reported: %d\n", err);
// }

// int main(void)
// {
//     // 设置错误报告函数
//     set_error_report_func(error_report);

//     printf("Starting scheduler with no tasks\n");
//     hyb_sch_start();
//     set_error_code(5);
//     printf("No tasks to dispatch\n");

//     for (int i = 0; i < 5; i++)
//     {
//         printf("\nTick %d\n", i);
//         hyb_sch_update();
//         hyb_sch_dispatch_tasks();
//     }
//     printf("err code = %d\n", get_error_code(5));
//     printf("err code mask = %d\n", get_error_code_mask());

//     return 0;
// }

// 测试 5、综合测试
// #include <stdio.h>
// #include "hyb_sched.h"

// // 全局错误报告函数
// void error_report(uint32_t err)
// {
//     printf("Error reported: %d\n", err);
// }

// // 低功耗模式函数
// void go_to_sleep(void)
// {
//     printf("System is going to sleep\n");
// }

// // 测试任务 1
// void task1(void)
// {
//     printf("Task 1 is running\n");
// }

// // 测试任务 2
// void task2(void)
// {
//     printf("Task 2 is running\n");
// }

// // 综合测试程序
// int main(void)
// {
//     // 设置错误报告和低功耗模式函数
//     set_error_report_func(error_report);
//     set_go_to_sleep_func(go_to_sleep);

//     // 创建任务
//     HYB_TASK *task1_handle = hyb_sch_create_task(task1, 1, 0, 0); // 单次任务，延迟 1
//     HYB_TASK *task2_handle = hyb_sch_create_task(task2, 2, 5, 1); // 周期任务，延迟 2，周期 5

//     // 启动调度器
//     hyb_sch_start();

//     // 打印初始任务列表
//     print_task_list("Initial task list");

//     // 运行调度器若干 tick
//     for (int i = 0; i < 10; i++)
//     {
//         printf("\nTick %d\n", i);
//         hyb_sch_update();
//         hyb_sch_dispatch_tasks();

//         // 打印当前任务列表
//         print_task_list("Current task list");

//         // 删除任务 1
//         if (i == 5)
//         { // 在第 5 个 tick 时删除任务 1
//             if (hyb_sch_delete_task(task1_handle) == 0)
//             {
//                 printf("Successfully deleted task 1\n");
//             }
//             else
//             {
//                 printf("Failed to delete task 1\n");
//             }
//         }
//     }

//     // 检查任务计数
//     printf("Task count before stopping: %d\n", hyb_sch_task_count());
//     // 停止调度器
//     hyb_sch_stop();

//     // 再次检查任务计数
//     printf("Task count after stopping: %d\n", hyb_sch_task_count());

//     return 0;
// }

// 测试 6、错误和警告报告测试
#include <stdio.h>
#include "hyb_sched.h"

static int ticks = 0;
HYB_TASK *task1_handle = NULL;
HYB_TASK *task2_handle = NULL;
HYB_TASK *task3_handle = NULL;

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
        printf("Task count: %d\n", hyb_sch_task_count());
        if (hyb_sch_delete_task(task2_handle) == 0)
        {
            printf("Successfully deleted Task 2 at %d ticks\n", ticks);
            print_task_list("Task List");
            printf("Task count: %d\n", hyb_sch_task_count());
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
    task1_handle = hyb_sch_create_task(task1, 1, 3, 1); // 周期任务，延迟 1，周期 3
    task2_handle = hyb_sch_create_task(task2, 2, 5, 1); // 周期任务，延迟 2，周期 5
    task3_handle = hyb_sch_create_task(task3, 0, 0, 0); // 单次任务，立即执行

    if (task1_handle == NULL || task2_handle == NULL || task3_handle == NULL)
    {
        printf("Failed to create tasks\n");
        return -1;
    }

    // 启动调度器
    hyb_sch_start();

    while (1)
    {
        hyb_sch_update();
        hyb_sch_dispatch_tasks();
        ticks++;

        // 设置一个停止条件，实际使用时是不需要的
        if (ticks > 20)
        {
            // 停止调度器
            hyb_sch_stop();
            break;
        }
    }

    return 0;
}
