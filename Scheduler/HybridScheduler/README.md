# Hybrid Scheduler 混合式调度器

## 项目简介

本项目实现了一个混合式调度器，适用于多任务环境下的任务切换。调度器通过轮询方式管理任务队列。

---

## 文件结构

- **`hyb_sched.h`**：调度器接口定义
- **`hyb_sched.c`**：调度器核心实现
- **`example_hyb_sched.c`**：调度器使用示例

---

## 功能特性

- **任务管理**：支持任务的创建、删除和调度。
- **低功耗模式**：支持进入低功耗模式。
- **错误与警告报告**：支持错误码和警告码的设置与报告。
- **任务队列**：支持动态任务链表管理，任务数量仅受限于系统内存。

---

## 使用方法

### 示例程序

运行 `example_hyb_sched.c` 示例程序，展示调度器的基本功能：

1. **创建三个任务**：
   - `task1`：周期任务，延迟 1 个时标，周期为 3。
   - `task2`：周期任务，延迟 2 个时标，周期为 5。
   - `task3`：单次任务，立即执行。
2. **模拟任务的调度与执行**：
   - 调度器会根据任务的延迟和周期动态调度任务。
3. **演示错误与警告报告功能**：
   - 示例程序展示了如何设置和获取错误码与警告码。

---

## 接口说明

以下是调度器的所有接口及其详细说明：

### 任务管理

- `HYB_TASK *hyb_sch_create_task(const void (*pFunction)(void), const uint16_t delay, const uint16_t cycle, const uint8_t coopFlag)`
  - **功能**：创建一个任务。
  - **参数**：
    - `pFunction`：任务函数指针。
    - `delay`：任务首次执行前的延迟时间。
    - `cycle`：任务循环执行的周期，0 表示单次任务。
    - `coopFlag`：任务调度标志：0 表示抢占式调度，1 表示合作式调度。
  - **返回值**：任务句柄，创建失败返回 `NULL`。

- `int hyb_sch_delete_task(const HYB_TASK *task_handle)`
  - **功能**：删除指定任务。
  - **参数**：
    - `task_handle`：任务句柄。
  - **返回值**：成功返回 `0`，失败返回 `-1`。

- `int hyb_sch_task_count(void)`
  - **功能**：获取当前任务数量。
  - **返回值**：当前任务数量。

### 调度器控制

- `void hyb_sch_update(void)`
  - **功能**：更新调度器状态，通常在定时器中断中调用。

- `void hyb_sch_dispatch_tasks(void)`
  - **功能**：调度任务，在主循环中调用。

- `void hyb_sch_start(void)`
  - **功能**：启动调度器。

- `void hyb_sch_stop(void)`
  - **功能**：停止调度器。

### 错误与警告管理

- `void set_error_report_func(const hyb_sch_report_func func)`
  - **功能**：设置错误报告函数。

- `void set_warning_report_func(const hyb_sch_report_func func)`
  - **功能**：设置警告报告函数。

- `void set_error_code(const uint8_t err_count)`
  - **功能**：设置错误码。

- `void reset_error_code(const uint8_t err_count)`
  - **功能**：清除错误码。

- `uint8_t get_error_code(const uint8_t err_count)`
  - **功能**：获取指定错误码。

- `uint32_t get_error_code_mask(void)`
  - **功能**：获取所有错误码。

- `void set_warning_code(const uint32_t warn)`
  - **功能**：设置警告码。

- `uint32_t get_warning_code(void)`
  - **功能**：获取警告码。

### 低功耗模式

- `void set_go_to_sleep_func(const hyb_sch_go_to_sleep_func func)`
  - **功能**：设置进入低功耗模式的函数。

---

## 配置选项

您可以通过修改 `hyb_sched.h` 文件中的宏定义调整调度器的行为：

- `HYB_SCH_REPORT_ERRORS`：启用或禁用错误报告。
- `HYB_SCH_REPORT_WARNINGS`：启用或禁用警告报告。
- `HYB_SCH_GO_TO_SLEEP`：启用或禁用低功耗模式。
- `HYB_SCH_REPORT_WARNINGS_TICKS`：设置警告报告的周期（单位：时标）。

---

## 注意事项

- 所有任务函数必须为非阻塞设计。
- 调度器必须对任务队列进行边界条件检查，防止空队列或无效任务导致崩溃。
- 任务函数执行时应捕获异常，确保调度器的稳定性。
- 只支持一个抢占式任务。

---

## 更新记录

- **v1.0.0**（2021-12-31）：初始版本发布。

---

## 贡献者

- **作者**: Jia Zhenyu
- **日期**: 2021-12-31
- **版本**: V1.0.0

---

## 许可证

该调度器为开源项目，用户可以自由使用、修改和分发。
