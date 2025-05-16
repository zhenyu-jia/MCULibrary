/**
 * @file    loopie_scheduler.h
 * @brief   调度器模块头文件，声明调度器相关接口与中断控制函数
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_SCHEDULER_H__
#define __LOOPIE_SCHEDULER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "loopie_config.h"
#include "loopie_task.h"
#include "loopie_event.h"
#include "loopie_event_ex.h"
#include "loopie_error.h"

#include "cmsis_compiler.h"

// #if defined(__CC_ARM) /* ARM Compiler V4/V5 */
// #include "cmsis_armcc.h"
// #elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) && (__ARMCC_VERSION < 6100100) /* Arm Compiler V6.6 LTM */
// #include "cmsis_armclang_ltm.h"
// #elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6100100) /* ARM Compiler V6.10.1 */
// #include "cmsis_armclang.h"
// #elif defined(__GNUC__) /* GNU Compiler */
// #include "cmsis_gcc.h"
// #elif defined(__ICCARM__) /* IAR Compiler */
// #include <cmsis_iccarm.h>
// #else
// #error "Unsupported compiler"
// #endif

    // 保存当前中断状态，并关闭中断（通过返回值保存）
    static inline uint32_t enter_critical(void)
    {
        uint32_t primask = __get_PRIMASK();
        __disable_irq();
        return primask;
    }

    // 保存当前中断状态，并关闭中断（通过指针保存）
    static inline void enter_critical_ptr(uint32_t *primask)
    {
        if (primask)
        {
            *primask = __get_PRIMASK();
            __disable_irq();
        }
    }

    // 恢复中断状态
    static inline void exit_critical(uint32_t primask)
    {
        __set_PRIMASK(primask);
    }

    // 关闭所有中断
    // static inline uint32_t enter_critical(void)
    // {
    //     __disable_irq();
    //     return 0;
    // }

    // 启用所有中断
    // static inline void exit_critical(uint32_t primask)
    // {
    //     __enable_irq();
    // }

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

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_SCHEDULER_H__ */
