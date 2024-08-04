// 测试 1、基本功能测试
// #include <stdio.h>
// #include "coop_sched.h"

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
//     cTask *task1_handle = cSch_create_task(task1, 1, 0); // 单次任务，延迟1
//     cTask *task2_handle = cSch_create_task(task2, 2, 5); // 周期任务，延迟2，周期5

//     cSch_start();

//     for (int i = 0; i < 10; i++)
//     {
//         printf("\nTick %d\n", i);
//         cSch_updata();
//         cSch_dispatch_tasks();
//     }

//     cSch_stop();

//     return 0;
// }

// 测试 2、任务删除测试
// #include <stdio.h>
// #include "coop_sched.h"

// void task1(void) {
//     printf("Task 1 is running\n");
// }

// void task2(void) {
//     printf("Task 2 is running\n");
// }

// int main(void) {
//     cTask *task1_handle = cSch_create_task(task1, 1, 0); // 单次任务
//     cTask *task2_handle = cSch_create_task(task2, 2, 5); // 周期任务

//     cSch_start();

//     for (int i = 0; i < 10; i++) {
//         printf("\nTick %d\n", i);
//         cSch_updata();
//         cSch_dispatch_tasks();

//         if (i == 5) { // 在第5个tick时删除任务1
//             if (cSch_delete_task(task1_handle) == 0) {
//                 printf("Successfully deleted task 1\n");
//             } else {
//                 printf("Failed to delete task 1\n");
//             }
//         }
//     }

//     cSch_stop();

//     return 0;
// }

// 测试 3、低功耗模式测试
// #include <stdio.h>
// #include "coop_sched.h"

// void task1(void) {
//     printf("Task 1 is running\n");
// }

// void go_to_sleep(void) {
//     printf("System is going to sleep\n");
// }

// int main(void) {
//     set_go_to_sleep(go_to_sleep);

//     cSch_create_task(task1, 1, 0);
//     cSch_start();

//     for (int i = 0; i < 5; i++) {
//         printf("\nTick %d\n", i);
//         cSch_updata();
//         cSch_dispatch_tasks();
//     }

//     cSch_stop();

//     return 0;
// }

// 测试 4、边界条件测试
// #include <stdio.h>
// #include "coop_sched.h"

// // 全局错误报告函数
// void error_report(uint32_t err) {
//     printf("Error reported: %d\n", err);
// }

// int main(void) {
//     // 设置错误报告函数
//     set_err_report(error_report);

//     printf("Starting scheduler with no tasks\n");
//     cSch_start();
//     set_err_code_mask(0x00000001);
//     printf("No tasks to dispatch\n");

//     for (int i = 0; i < 5; i++) {
//         printf("\nTick %d\n", i);
//         cSch_updata();
//         cSch_dispatch_tasks();
//     }
//     printf("err code mask = %d\n", get_err_code_mask());
//     cSch_stop();

//     return 0;
// }

// 测试 5、综合测试
#include <stdio.h>
#include "coop_sched.h"

// 全局错误报告函数
void error_report(uint32_t err) {
    printf("Error reported: %d\n", err);
}

// 低功耗模式函数
void go_to_sleep(void) {
    printf("System is going to sleep\n");
}

// 测试任务1
void task1(void) {
    printf("Task 1 is running\n");
}

// 测试任务2
void task2(void) {
    printf("Task 2 is running\n");
}

// 综合测试程序
int main(void) {
    // 设置错误报告和低功耗模式函数
    set_err_report(error_report);
    set_go_to_sleep(go_to_sleep);

    // 创建任务
    cTask *task1_handle = cSch_create_task(task1, 1, 0); // 单次任务，延迟1
    cTask *task2_handle = cSch_create_task(task2, 2, 5); // 周期任务，延迟2，周期5

    // 启动调度器
    cSch_start();

    // 打印初始任务列表
    print_task_list("Initial task list");

    // 运行调度器若干tick
    for (int i = 0; i < 10; i++) {
        printf("\nTick %d\n", i);
        cSch_updata();
        cSch_dispatch_tasks();

        // 打印当前任务列表
        print_task_list("Current task list");

        // 删除任务1
        if (i == 5) { // 在第5个tick时删除任务1
            if (cSch_delete_task(task1_handle) == 0) {
                printf("Successfully deleted task 1\n");
            } else {
                printf("Failed to delete task 1\n");
            }
        }
    }

    // 检查任务计数
    printf("Task count before stopping: %d\n", cSch_task_count());
    // 停止调度器
    cSch_stop();

    // 再次检查任务计数
    printf("Task count after stopping: %d\n", cSch_task_count());

    return 0;
}
