/**
 *******************************************************************************
 * @file    coop_sched.h
 * @author  Jia Zhenyu
 * @version V1.0.0
 * @date    2021-12-25
 * @brief   合作式调度器头文件
 *
 * @details 本文件提供了合作式调度器的接口，适用于多任务环境下的任务切换。
 *          调度器通过轮询方式管理任务队列。
 *
 * @attention
 *          - 使用时需确保所有任务函数均为非阻塞设计。
 *          - 调度器必须对任务队列进行边界条件检查，防止空队列或无效任务导致崩溃。
 *          - 任务函数执行时应捕获异常，确保调度器的稳定性。
 *
 * @copyright
 *          Copyright (C) 2021 JZY. All rights reserved.
 *          This software is licensed under the MIT License.
 *
 * @configuration
 *          - CO_SCH_MAX_TASKS: 最大任务数，可通过宏定义调整。
 *          - CO_SCH_REPORT_ERRORS: 启用错误报告，注释掉以禁用。
 *          - CO_SCH_REPORT_WARNINGS: 启用警告报告，注释掉以禁用。
 *          - CO_SCH_GO_TO_SLEEP: 允许系统进入低功耗模式，注释掉以禁用。
 *          - CO_SCH_REPORT_WARNINGS_TICKS: 警告代码报告周期，单位为时标。
 *******************************************************************************
 */

#ifndef __COOP_SCHED_H__
#define __COOP_SCHED_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* 公用的常数 ---------------------------------------------------------------*/
/**
 * @brief 任务最大数量
 * @note 每个新建项目都必须修改此值
 */
#define CO_SCH_MAX_TASKS 10U

/**
 * @brief 启用错误报告
 * @note 注释掉以禁用错误报告
 */
#define CO_SCH_REPORT_ERRORS

/**
 * @brief 启用警告报告
 * @note 注释掉以禁用警告报告
 */
#define CO_SCH_REPORT_WARNINGS

/**
 * @brief 允许系统进入低功耗模式
 * @note 注释掉以禁用低功耗模式
 */
#define CO_SCH_GO_TO_SLEEP

/**
 * @brief 警告代码报告周期
 * @note 单位为时标
 */
#define CO_SCH_REPORT_WARNINGS_TICKS 6000U

/* 以下内容不需要修改 */
/* 检查 CO_SCH_REPORT_WARNINGS_TICKS 是否大于 0 */
#ifdef CO_SCH_REPORT_WARNINGS_TICKS
#if CO_SCH_REPORT_WARNINGS_TICKS < 1U
#error "CO_SCH_REPORT_WARNINGS_TICKS must be greater than 0"
#endif /* CO_SCH_REPORT_WARNINGS_TICKS < 1U */
#endif /* CO_SCH_REPORT_WARNINGS_TICKS */

/* 错误码，用掩码的方式定义，同时能够处理 32 个错误码 */
#define NO_ERROR_MASK 0U

/* 警告码，是数值，不是掩码 */
#define NO_WARNING 0U

/* 错误码 */
#define ERROR_TASK_QUEUE_FULL -1 // 任务队列已满
#define ERROR_TASK_NOT_FOUND -2  // 任务未找到（无法删除）
#define ERROR_INVALID_PARAM -3   // 无效的参数（例如无效的函数指针）

    /* 公用的数据类型 -----------------------------------------------------------*/
    /**
     * @brief  任务数据类型
     * @details 每个任务的存储器的总和是 12 个字节
     */
    typedef struct __CO_TASK
    {
        uint16_t runFlag;    // 任务运行标志
        uint16_t delay;      // 延迟（时标）
        uint16_t cycle;      // 周期（时标）
        void (*pTask)(void); // 指向任务的指针（必须是一个 "void(void)" 函数）
    } CO_TASK;

    /**
     * @brief  进入低功耗模式的函数指针类型
     */
    typedef void (*co_sch_go_to_sleep_func)(void);

    /**
     * @brief  错误/警告报告函数的函数指针类型
     */
    typedef void (*co_sch_report_func)(uint32_t);

    /* 公用的函数原型 -----------------------------------------------------------*/
    /**
     * @brief 调度器内核函数
     */
    int co_sch_create_task(const void (*pFunction)(void), const uint16_t delay, const uint16_t cycle);
    int co_sch_delete_task(const int task_index);
    void co_sch_update(void);
    void co_sch_dispatch_tasks(void);
    void co_sch_start(void);
    void co_sch_stop(void);
    int co_sch_task_count(void);
    void set_error_report_func(const co_sch_report_func func);
    void set_warning_report_func(const co_sch_report_func func);
    void set_go_to_sleep_func(const co_sch_go_to_sleep_func func);
    void set_error_code(const uint8_t err_count);
    void reset_error_code(const uint8_t err_count);
    uint8_t get_error_code(const uint8_t err_count);
    uint32_t get_error_code_mask(void);
    void set_warning_code(const uint32_t warn);
    uint32_t get_warning_code(void);
    void print_task_list(const char *label);

#ifdef __cplusplus
}
#endif

#endif /* __COOP_SCHED_H__ */
