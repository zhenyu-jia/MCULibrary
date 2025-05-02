/**
 * @file slow_pwm.c
 * @brief 实现低速 PWM 控制功能的操作函数。
 *
 * 该文件实现了低速 PWM（脉冲宽度调制）控制的相关操作函数。包括 PWM 结构体的初始化、速度设置、
 * 周期数设置、PWM 启动、停止以及状态更新等功能。
 *
 * @note
 * - `slow_pwm_init`：初始化 PWM 结构体。
 * - `slow_pwm_set_speed`：设置 PWM 的高电平和低电平时间。
 * - `slow_pwm_set_num`：设置 PWM 的周期数。
 * - `slow_pwm_start`：启动 PWM 并设置速度和周期数。
 * - `slow_pwm_stop`：停止 PWM。
 * - `slow_pwm_ticks`：更新 PWM 状态，处理 PWM 信号的周期。
 *
 * @version 1.0.0
 * @date 2024-07-31
 * @author Jia Zhenyu
 *
 * @par Example
 * @code
 * void set_pwm_level(SLOW_PWM *hpwm, uint8_t level)
 * {
 *     // 设置 PWM 输出电平的实际实现
 * }
 *
 * SLOW_PWM pwm;
 * slow_pwm_init(&pwm, set_pwm_level);
 * slow_pwm_start(&pwm, 100, 200, 10);
 *
 * while (1)
 * {
 *     slow_pwm_ticks(&pwm);
 *     // 其他代码
 * }
 * @endcode
 */

#include "slow_pwm.h"

/**
 * @brief 初始化 PWM 结构体。
 *
 * 该函数初始化 PWM 结构体，并设置用于设置 PWM 电平的回调函数。
 *
 * @param hpwm 指向要初始化的 PWM 结构体的指针。
 * @param set_pwm_level 设置 PWM 电平的回调函数。
 */
void slow_pwm_init(SLOW_PWM *hpwm, void (*set_pwm_level)(SLOW_PWM *, uint8_t))
{
    memset(hpwm, 0, sizeof(SLOW_PWM));
    hpwm->set_pwm_level = set_pwm_level;
}

/**
 * @brief 设置 PWM 速度。
 *
 * 该函数设置 PWM 的高电平和低电平时间。
 *
 * @param hpwm 指向 PWM 结构体的指针。
 * @param ticks_low 高电平时间（单位：时钟周期）。
 * @param ticks_high 低电平时间（单位：时钟周期）。
 */
void slow_pwm_set_speed(SLOW_PWM *hpwm, uint16_t ticks_low, uint16_t ticks_high)
{
    hpwm->ticks_low = ticks_low;
    hpwm->ticks_high = ticks_high;
}

/**
 * @brief 设置 PWM 周期数。
 *
 * 该函数设置 PWM 的周期数，即 PWM 信号的总周期次数。
 *
 * @param hpwm 指向 PWM 结构体的指针。
 * @param ticks_num PWM 周期数。
 */
void slow_pwm_set_num(SLOW_PWM *hpwm, uint8_t ticks_num)
{
    if (ticks_num > PWM_TICKS_NUM_MAX)
    {
        ticks_num = PWM_TICKS_NUM_MAX;
    }
    hpwm->ticks_num = ticks_num;
}

/**
 * @brief 启动 PWM。
 *
 * 该函数启动 PWM 并设置 PWM 的速度和周期数。
 *
 * @param hpwm 指向 PWM 结构体的指针。
 * @param ticks_low 高电平时间（单位：时钟周期）。
 * @param ticks_high 低电平时间（单位：时钟周期）。
 * @param ticks_num PWM 周期数。
 */
void slow_pwm_start(SLOW_PWM *hpwm, uint16_t ticks_low, uint16_t ticks_high, uint8_t ticks_num)
{
    slow_pwm_set_speed(hpwm, ticks_low, ticks_high);
    slow_pwm_set_num(hpwm, ticks_num);
    hpwm->current_level = 0;
}

/**
 * @brief 停止 PWM。
 *
 * 该函数停止 PWM，清除 PWM 的速度和周期数。
 *
 * @param hpwm 指向 PWM 结构体的指针。
 */
void slow_pwm_stop(SLOW_PWM *hpwm)
{
    slow_pwm_set_speed(hpwm, 0, 0);
    slow_pwm_set_num(hpwm, 0);
    hpwm->current_level = 0;
}

/**
 * @brief 更新 PWM 状态。
 *
 * 该函数更新 PWM 信号的状态，根据设置的高电平和低电平时间处理 PWM 信号的周期。
 *
 * @param hpwm 指向 PWM 结构体的指针。
 */
void slow_pwm_ticks(SLOW_PWM *hpwm)
{
    uint8_t pre_level = hpwm->current_level;

    // 只有在 PWM 周期数大于 0 时才进行处理
    if (hpwm->ticks_num > 0)
    {
        if (!hpwm->current_level)
        {
            if (hpwm->ticks >= hpwm->ticks_low)
            {
                hpwm->current_level = 1;
                hpwm->ticks = 0;
            }
        }
        else
        {
            if (hpwm->ticks >= hpwm->ticks_high)
            {
                hpwm->current_level = 0;
                hpwm->ticks = 0;
                if (hpwm->ticks_num < PWM_TICKS_NUM_MAX)
                {
                    hpwm->ticks_num--;
                }
            }
        }
    }
    else
    {
        slow_pwm_stop(hpwm);
    }

    // 如果电平发生变化，则调用设置 PWM 电平的回调函数
    if (pre_level != hpwm->current_level)
    {
        hpwm->set_pwm_level(hpwm, hpwm->current_level);
    }
    hpwm->ticks++;
}
