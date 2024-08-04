/**
 ******************************************************************************
 * @file    coop_sched.h
 * @author  JZY
 * @version V1.0.0
 * @date    2021-12-25
 * @brief   None
 ******************************************************************************
 * @attention
 *
 * None
 ******************************************************************************
 */

#ifndef __COOP_SCHED_H__
#define __COOP_SCHED_H__

#include <stdint.h>

/* 公用的常数 ---------------------------------------------------------------*/
// 错误码，用掩码的方式定义，同时能够处理32个错误码
#define NO_ERROR_MASK (0x00000000)

/* 公用的数据类型 -----------------------------------------------------------*/
/**
 * @brief  任务数据类型
 *
 * 任务结构体包含了任务的运行标志、延迟、周期和任务函数指针。
 * 每个任务的存储器总和是 16 个字节。
 */
typedef struct __cTask
{
    uint8_t runFlag;     // 任务运行标志
    uint16_t delay;      // 延迟（时标）
    uint16_t cycle;      // 周期（时标）
    void (*pTask)(void); // 指向任务的指针（必须是一个 "void(void)" 函数）
    struct __cTask *next;
} cTask;

/**
 * @brief  进入低功耗模式的函数指针类型
 */
typedef void (*cSch_go_to_sleep_func)(void);
/**
 * @brief  错误报告函数的函数指针类型
 *
 * @param  error_mask  错误码掩码
 */
typedef void (*cSch_err_report_func)(uint32_t);

/* 公用的函数原型 -----------------------------------------------------------*/
/*
 * 调度器内核函数
 */
cTask *cSch_create_task(void (*pFunction)(void), const uint16_t delay, const uint16_t cycle);
int cSch_delete_task(cTask *task_handle);
void cSch_updata(void);
void cSch_dispatch_tasks(void);
void cSch_start(void);
void cSch_stop(void);
int cSch_task_count(void);
void set_err_report(cSch_err_report_func func);
void set_go_to_sleep(cSch_go_to_sleep_func func);
void set_err_code_mask(uint32_t err_mask);
uint32_t get_err_code_mask(void);
void print_task_list(const char *label);
#endif /* __COOP_SCHED_H__ */
