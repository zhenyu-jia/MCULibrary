# Loopie 任务调度与事件管理系统

## 简介

Loopie 是一个轻量级的任务调度与事件管理系统，适用于嵌入式和通用 C 项目。它支持任务的创建、删除、挂起、恢复、调度，支持事件队列、事件标志（含回调）、错误与警告码管理等功能。

---

## 主要特性

- **任务管理**：支持延迟、周期、单次/周期任务。
- **事件队列**：支持事件投递、覆盖/丢弃模式，支持中断安全。
- **事件标志**：支持普通事件标志和带回调的事件标志。
- **错误与警告管理**：支持错误码/警告码的设置、清除、报告。
- **可配置**：所有参数均可在 [`loopie_config.h`](loopie_config.h) 中配置。

---

## 目录结构

```bash
loopie_config.h         // 配置文件
loopie_critical.h       // 临界区管理
loopie_error.c/h        // 错误与警告处理
loopie_event_ex.c/h     // 事件标志与回调
loopie_event.c/h        // 事件队列
loopie_scheduler.c/h    // 调度器
loopie_task.c/h         // 任务管理
main.c                  // 示例主程序
```

---

## 配置宏说明（摘自 [`loopie_config.h`](loopie_config.h)）

- `SCH_TASK_MAX_NUM`：最大任务数量
- `SCH_TASK_MAX_RUN_FLAG`：任务最大运行标志（最大可运行次数，防止 runFlag 溢出）
- `SCH_EVENT`：启用事件功能
- `SCH_EVENT_MAX_NUM`：事件队列最大数量（必须为 2 的幂，决定事件队列长度）
- `SCH_EVENT_MASK`：事件队列索引掩码（用于环形队列）
- `SCH_EVENT_EX`：启用事件扩展
- `SCH_EVENT_MAX_FLAG`：事件标志最大数量（普通事件标志位数）
- `SCH_EVENT_MAX_FLAG_CB`：带回调事件标志最大数量
- `SCH_REPORT_ERRORS`：启用错误代码报告
- `SCH_REPORT_WARNINGS`：启用警告代码报告
- `SCH_REPORT_WARNINGS_TICKS`：警告代码报告周期（单位：调度周期数）

---

## 主要接口函数

### 任务管理（[`loopie_task.h`](loopie_task.h)）

```c
void task_init(void);
int task_create(void (*const pTask)(void *), void *const arg, const uint16_t delay, const uint16_t cycle);
int task_delete(const int task_index);
int task_suspend(const int task_index);
int task_resume(const int task_index);
void task_update(void);
void task_run(void);
int task_get_count(void);
uint32_t task_get_interval(const int task_index);
```

### 事件队列（[`loopie_event.h`](loopie_event.h)）

```c
void event_init(void);
int event_post(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode);
int event_post_from_isr(void (*const pEvent)(void *), void *const arg, const EVENT_POST_MODE mode);
int event_post_default(void (*const pEvent)(void *));
int event_post_from_isr_default(void (*const pEvent)(void *));
uint32_t event_queue_free_size(void);
void event_run(void);
```

### 事件标志（[`loopie_event_ex.h`](loopie_event_ex.h)）

```c
int event_flag_set(const uint32_t flag_count);
int event_flag_cb_set(const uint32_t flag_count);
int event_flag_clear(const uint32_t flag_count);
int event_flag_cb_clear(const uint32_t flag_count);
uint8_t event_flag_get(const uint32_t flag_count);
uint8_t event_flag_cb_get(const uint32_t flag_count);
void event_flag_clear_all(void);
void event_flag_cb_clear_all(void);
void event_flag_cb_set_callback(void (*const callback)(uint32_t));
void event_flag_cb_process(void);
```

### 错误与警告（[`loopie_error.h`](loopie_error.h)）

```c
int error_code_set(const uint8_t err_count);
int error_code_clear(const uint8_t err_count);
void error_code_clear_all(void);
uint8_t error_code_get(const uint8_t err_count);
uint32_t error_code_get_all(void);
uint32_t warning_code_set(const uint32_t warn);
uint32_t warning_code_get(void);
void error_set_report_func(report_func func);
void warning_set_report_func(report_func func);
void error_report(void);
void warning_report(void);
```

### 调度器（[`loopie_scheduler.h`](loopie_scheduler.h)）

```c
void scheduler_set_idle_hook(void (*const hook)(void));
void scheduler_set_time_func(uint32_t (*const func)(void));
void scheduler_suspend(void);
void scheduler_resume(void);
void scheduler_start(void);
void scheduler_stop(void);
void scheduler_restart(void);
void scheduler_init(void);
void scheduler_update(void);
void scheduler_run(void);
uint32_t scheduler_get_duration(void);
```

---

## 示例代码

以下为主流程示例，更多细节见 [`main.c`](main.c)：

```c
#include "loopie_scheduler.h"

uint32_t ticks = 0;

void task1(void *arg) { /* ... */ }
void my_idle_handler(void) { /* ... */ }
uint32_t my_get_system_ticks(void) { return ticks; }
void my_error_report(uint32_t error_code) { /* ... */ }
void my_warning_report(uint32_t warning_code) { /* ... */ }
void my_event_handler(uint32_t flag_count) { /* ... */ }

int main()
{
    // 设置回调
    error_set_report_func(my_error_report);
    warning_set_report_func(my_warning_report);
    event_flag_cb_set_callback(my_event_handler);
    scheduler_set_idle_hook(my_idle_handler);
    scheduler_set_time_func(my_get_system_ticks);

    scheduler_init();

    // 创建任务
    int task1_index = task_create(task1, (char *)"task1", 1, 1);

    scheduler_start();

    while (1)
    {
        scheduler_update();
        scheduler_run();

        // 事件、标志、错误、警告等操作
        // ...
        ticks++;
    }
    return 0;
}
```

---

## 事件队列用法示例

```c
void buttonHandler(void *arg) { /* ... */ }
int btn_arg = 123;
event_post(buttonHandler, &btn_arg, EVENT_POST_DISCARD);
```

## 事件标志用法示例

```c
event_flag_set(3); // 设置第 3 号标志
if (event_flag_get(3)) {
    event_flag_clear(3);
}
```

## 错误与警告用法示例

```c
error_code_set(0);      // 设置错误码 0
error_code_clear(0);    // 清除错误码 0
warning_code_set(1);    // 设置警告码 1
```

---

## 参考

- 各模块头文件有详细注释和接口说明。
- 详细示例见 [`main.c`](main.c)。

---

## 更新记录

- **v1.0.0**（2024-08-01）：初始版本发布。

---

## 贡献者

- **作者**: Jia Zhenyu
- **日期**: 2024-08-01
- **版本**: V1.1.0

---

## 许可证

该调度器为开源项目，用户可以自由使用、修改和分发。
