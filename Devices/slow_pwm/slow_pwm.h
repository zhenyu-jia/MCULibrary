/**
 * @file slow_pwm.h
 * @brief 低速 PWM (脉宽调制) 控制库头文件。
 *
 * 该文件定义了低速 PWM 控制的结构体和函数接口。
 * 本文件提供了 PWM 初始化、设置、启动、停止及时间周期处理等功能的接口。
 *
 * @note
 * - `SLOW_PWM` 结构体：表示 PWM 控制的状态和参数。
 * - `slow_pwm_init`：初始化 PWM 控制结构体。
 * - `slow_pwm_set_speed`：设置 PWM 的高电平和低电平时间。
 * - `slow_pwm_set_num`：设置 PWM 周期内的循环次数。
 * - `slow_pwm_start`：启动 PWM 输出。
 * - `slow_pwm_stop`：停止 PWM 输出。
 * - `slow_pwm_ticks`：更新 PWM 状态和控制信号。
 *
 * @version 1.0.0
 * @date 2024-07-31
 * @author Jia Zhenyu
 *
 * @par Example
 * @code
 * void pwm_set_level(SLOW_PWM *hpwm, uint8_t level) {
 *     // 设置 PWM 输出电平
 * }
 *
 * SLOW_PWM pwm;
 * slow_pwm_init(&pwm, pwm_set_level);
 * slow_pwm_start(&pwm, 1000, 2000, 10);
 *
 * while (1) {
 *     slow_pwm_ticks(&pwm); // 在主循环中定期调用以更新 PWM 输出
 * }
 * @endcode
 */

#ifndef __SLOW_PWM_H__
#define __SLOW_PWM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

// #define PWM_TICKS_INTERVAL 5U // 时间间隔 (ms)
#define PWM_TICKS_NUM_MAX 0x7FU

    /**
     * @brief 低速 PWM 控制结构体。
     *
     * 该结构体用于表示 PWM 控制的状态和参数，包括当前电平、PWM 状态、循环次数及时间周期等。
     */
    typedef struct __SLOW_PWM
    {
        uint8_t current_level : 1;
        uint8_t ticks_num : 7; // 循环次数
        uint16_t ticks;
        uint16_t ticks_high; // 一个周期内高电平的时间 (PWM_TICKS_INTERVAL)
        uint16_t ticks_low;  // 一个周期内低电平的时间 (PWM_TICKS_INTERVAL)
        void (*set_pwm_level)(struct __SLOW_PWM *hpwm, uint8_t level);
    } SLOW_PWM;

    void slow_pwm_init(SLOW_PWM *hpwm, void (*set_pwm_level)(SLOW_PWM *, uint8_t));                  /* 始化 PWM 控制结构体 */
    void slow_pwm_set_speed(SLOW_PWM *hpwm, uint16_t ticks_low, uint16_t ticks_high);                /* 设置 PWM 的高电平和低电平时间 */
    void slow_pwm_set_num(SLOW_PWM *hpwm, uint8_t ticks_num);                                        /* 设置 PWM 周期内的循环次数 */
    void slow_pwm_start(SLOW_PWM *hpwm, uint16_t ticks_low, uint16_t ticks_high, uint8_t ticks_num); /* 启动 PWM 输出 */
    void slow_pwm_stop(SLOW_PWM *hpwm);                                                              /* 停止 PWM 输出 */
    void slow_pwm_ticks(SLOW_PWM *hpwm);                                                             /* 更新 PWM 状态和控制信号 */

#ifdef __cplusplus
}
#endif

#endif /* __SLOW_PWM_H__ */
