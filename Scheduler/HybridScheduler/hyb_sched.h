/**
 *******************************************************************************
 * @file    hyb_sched.h
 * @author  Jia Zhenyu
 * @version V1.0.0
 * @date    2021-12-31
 * @brief   混合式调度器头文件
 *
 * @details 本文件提供了混合式调度器的接口，适用于多任务环境下的任务切换。
 *          调度器通过轮询方式管理任务队列。
 *
 * @attention
 *          - 使用时需确保所有任务函数均为非阻塞设计。
 *          - 调度器必须对任务队列进行边界条件检查，防止空队列或无效任务导致崩溃。
 *          - 任务函数执行时应捕获异常，确保调度器的稳定性。
 *          - 只支持一个抢占式任务。
 *
 * @copyright
 *          Copyright (C) 2021 JZY. All rights reserved.
 *          This software is licensed under the MIT License.
 *
 * @configuration
 *          - HYB_SCH_REPORT_ERRORS: 启用错误报告，注释掉以禁用。
 *          - HYB_SCH_REPORT_WARNINGS: 启用警告报告，注释掉以禁用。
 *          - HYB_SCH_GO_TO_SLEEP: 允许系统进入低功耗模式，注释掉以禁用。
 *          - HYB_SCH_REPORT_WARNINGS_TICKS: 警告代码报告周期，单位为时标。
 *******************************************************************************
 */

#ifndef __HYB_SCHED_H__
#define __HYB_SCHED_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes -----------------------------------------------------------------*/
#include <stdint.h>

/* 公用的常数 ---------------------------------------------------------------*/
/**
 * @brief 启用错误报告
 * @note 注释掉以禁用错误报告
 */
#define HYB_SCH_REPORT_ERRORS

/**
 * @brief 启用警告报告
 * @note 注释掉以禁用警告报告
 */
#define HYB_SCH_REPORT_WARNINGS

/**
 * @brief 允许系统进入低功耗模式
 * @note 注释掉以禁用低功耗模式
 */
#define HYB_SCH_GO_TO_SLEEP

/**
 * @brief 警告代码报告周期
 * @note 单位为时标
 */
#define HYB_SCH_REPORT_WARNINGS_TICKS 6000U

/* 以下内容不需要修改 */
/* 检查 HYB_SCH_REPORT_WARNINGS_TICKS 是否大于 0 */
#ifdef HYB_SCH_REPORT_WARNINGS_TICKS
#if HYB_SCH_REPORT_WARNINGS_TICKS < 1U
#error "HYB_SCH_REPORT_WARNINGS_TICKS must be greater than 0"
#endif /* HYB_SCH_REPORT_WARNINGS_TICKS < 1U */
#endif /* HYB_SCH_REPORT_WARNINGS_TICKS */

/* 错误码，用掩码的方式定义，同时能够处理 32 个错误码 */
#define NO_ERROR_MASK 0U

/* 警告码，是数值，不是掩码 */
#define NO_WARNING 0U

    /* 公用的数据类型 -----------------------------------------------------------*/
    /**
     * @brief  任务数据类型
     * @details 每个任务的存储器的总和是 16 个字节
     */
    typedef struct __HYB_TASK
    {
        uint16_t coopFlag : 1; // 合作式调度标志，合作式调度为 1，抢占式调度为 0
        uint16_t runFlag : 15; // 任务运行标志
        uint16_t delay;        // 延迟（时标）
        uint16_t cycle;        // 周期（时标）
        void (*pTask)(void);   // 指向任务的指针（必须是一个 "void(void)" 函数）
        struct __HYB_TASK *next;
    } HYB_TASK;

    /**
     * @brief  进入低功耗模式的函数指针类型
     */
    typedef void (*hyb_sch_go_to_sleep_func)(void);

    /**
     * @brief  错误/警告报告函数的函数指针类型
     */
    typedef void (*hyb_sch_report_func)(uint32_t);

    /* 公用的函数原型 -----------------------------------------------------------*/
    /**
     * @brief 调度器内核函数
     */
    HYB_TASK *hyb_sch_create_task(const void (*pFunction)(void), const uint16_t delay, const uint16_t cycle, const uint8_t coopFlag);
    int hyb_sch_delete_task(const HYB_TASK *task_handle);
    void hyb_sch_update(void);
    void hyb_sch_dispatch_tasks(void);
    void hyb_sch_start(void);
    void hyb_sch_stop(void);
    int hyb_sch_task_count(void);
    void set_error_report_func(const hyb_sch_report_func func);
    void set_warning_report_func(const hyb_sch_report_func func);
    void set_go_to_sleep_func(const hyb_sch_go_to_sleep_func func);
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

#endif /* __HYB_SCHED_H__ */
