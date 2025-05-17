/**
 * @file    loopie_critical.h
 * @brief   临界区管理模块头文件，提供中断屏蔽与恢复的接口和相关宏
 *          可用于多平台，支持不同编译器的中断控制，适合任务/事件等临界区保护
 * @author  Jia Zhenyu
 * @date    2024-08-01
 * @version 1.0.0
 */

#ifndef __LOOPIE_CRITICAL_H__
#define __LOOPIE_CRITICAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
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

// 宏封装（更简洁）（支持嵌套）
#define CRITICAL_SECTION() for (uint32_t _primask = enter_critical(); _primask != 0xFFFFFFFFU; exit_critical(_primask), _primask = 0xFFFFFFFFU)

    /* 支持嵌套 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
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

    /* 不支持嵌套 vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv */
    // // 关闭所有中断
    // static inline uint32_t enter_critical(void)
    // {
    //     __disable_irq();
    //     return 0;
    // }

    // // 启用所有中断
    // static inline void exit_critical(uint32_t primask)
    // {
    //     __enable_irq();
    // }

#ifdef __cplusplus
}
#endif

#endif /* __LOOPIE_CRITICAL_H__ */
