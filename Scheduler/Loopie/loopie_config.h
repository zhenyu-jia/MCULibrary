/**
 * @file    loopie_config.h
 * @brief   配置文件，定义任务、事件、错误等相关参数的宏
 *          重要的宏说明如下：
 *          - SCH_MAX_TASK_NUM:         最大任务数量
 *          - SCH_EVENT:                启用事件功能
 *          - SCH_MAX_EVENT_NUM:        事件队列最大数量（必须为2的幂）
 *          - SCH_EVENT_EX:             启用事件扩展功能
 *          - SCH_MAX_EVENT_FLAG:       事件标志最大数量
 *          - SCH_MAX_EVENT_FLAG_CB:    带回调的事件标志最大数量
 *          - SCH_REPORT_ERRORS:        启用错误代码报告
 *          - SCH_REPORT_WARNINGS:      启用警告代码报告
 *
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_CONFIG_H__
#define __LOOPIE_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif

// task 设置
#define SCH_MAX_TASK_NUM 50U       // 任务最大数量
#define SCH_MAX_TASK_RUN_FLAG 127U // 任务最大运行标志，最大运行标志为 127
#define SCH_USER_PRIORITY_MIN 0    // 任务最小优先级
#define SCH_USER_PRIORITY_MAX 127  // 任务最大优先级，最大优先级为 127

// event 设置
#define SCH_EVENT                               // 启用事件
#define SCH_MAX_EVENT_NUM 8U                    // 事件最大数量，2 的幂
#define SCH_EVENT_MASK (SCH_MAX_EVENT_NUM - 1U) // 事件队列大小掩码

// event_ex 设置
#define SCH_EVENT_EX              // 启用事件扩展
#define SCH_MAX_EVENT_FLAG 32U    // 事件标志最大数量
#define SCH_MAX_EVENT_FLAG_CB 64U // 带回调函数的事件标志最大数量

// error 设置
#define SCH_REPORT_ERRORS               // 启用错误代码报告
#define SCH_REPORT_WARNINGS             // 启用警告代码报告
#define SCH_REPORT_WARNINGS_TICKS 6000U // 警告代码报告周期

// 宏检查
#ifdef SCH_MAX_TASK_RUN_FLAG // 任务运行标志数量检查
#if SCH_MAX_TASK_RUN_FLAG > 127U
#error "SCH_MAX_TASK_RUN_FLAG must be less than or equal to 127U"
#endif // SCH_MAX_TASK_RUN_FLAG > 127U
#endif // SCH_MAX_TASK_RUN_FLAG

#ifdef SCH_USER_PRIORITY_MAX // 用户优先级数量检查
#if SCH_USER_PRIORITY_MAX > 127
#error "SCH_USER_PRIORITY_MAX must be less than or equal to 127"
#endif // SCH_USER_PRIORITY_MAX > 127
#endif // SCH_USER_PRIORITY_MAX

#ifdef SCH_MAX_EVENT_NUM // 事件数量检查
#if SCH_MAX_EVENT_NUM < 1U
#error "SCH_MAX_EVENT_NUM must be greater than or equal to 1U"
#endif // SCH_MAX_EVENT_NUM < 1U
#if (SCH_MAX_EVENT_NUM & SCH_EVENT_MASK) != 0
#error "SCH_MAX_EVENT_NUM must be a power of 2"
#endif // (SCH_MAX_EVENT_NUM & SCH_EVENT_MASK) != 0
#endif // SCH_MAX_EVENT_NUM

#ifdef SCH_REPORT_WARNINGS_TICKS // 检查 SCH_REPORT_WARNINGS_TICKS 是否大于 0
#if SCH_REPORT_WARNINGS_TICKS < 1U
#error "SCH_REPORT_WARNINGS_TICKS must be greater than or equal to 1U"
#endif /* SCH_REPORT_WARNINGS_TICKS < 1U */
#endif /* SCH_REPORT_WARNINGS_TICKS */

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_CONFIG_H__ */
