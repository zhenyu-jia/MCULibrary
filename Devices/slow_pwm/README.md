# Slow PWM 控制库

该项目实现了一个低速 PWM（脉宽调制）控制库，提供了 PWM 初始化、设置、启动、停止及时间周期处理等功能。

## 文件结构

```cc
slow_pwm.c
slow_pwm.h
example_slow_pwm.c
```

## 功能概述

- **PWM 初始化**：通过 `slow_pwm_init` 函数初始化 PWM 控制结构体。
- **设置 PWM 速度**：通过 `slow_pwm_set_speed` 设置高电平和低电平时间。
- **设置 PWM 周期数**：通过 `slow_pwm_set_num` 设置 PWM 信号的总周期次数。
- **启动 PWM**：通过 `slow_pwm_start` 启动 PWM 输出。
- **停止 PWM**：通过 `slow_pwm_stop` 停止 PWM 输出。
- **更新 PWM 状态**：通过 `slow_pwm_ticks` 更新 PWM 信号状态。

## 示例代码

以下是如何使用 Slow PWM 控制库的示例代码，详见 [example_slow_pwm.c](example_slow_pwm.c)：

```c
#include "slow_pwm.h"

void set_pwm_level(SLOW_PWM *hpwm, uint8_t level) {
    // 设置 PWM 输出电平
}

int main(void) {
    SLOW_PWM pwm;
    slow_pwm_init(&pwm, set_pwm_level);
    slow_pwm_start(&pwm, 1000, 2000, 10);

    while (1) {
        slow_pwm_ticks(&pwm); // 定期调用以更新 PWM 输出
    }
    return 0;
}
```

## 使用说明

1. **初始化 PWM**
调用 `slow_pwm_init` 函数初始化 `SLOW_PWM` 结构体，并传入设置 PWM 电平的回调函数。

2. **设置 PWM 参数**
使用 `slow_pwm_set_speed` 设置高电平和低电平时间，使用 `slow_pwm_set_num` 设置周期数。

3. **启动 PWM**
调用 `slow_pwm_start` 启动 PWM 输出。

4. **更新 PWM 状态**
在主循环或定时器中调用 `slow_pwm_ticks` 更新 PWM 状态。

5. **停止 PWM**
调用 `slow_pwm_stop` 停止 PWM 输出。

## 注意事项

`slow_pwm_set_num` 设置的周期数如果等于 `0x7F`, 则表示 PWM 会一直运行，直到调用 `slow_pwm_stop` 停止。

## 项目作者

- 作者：Jia Zhenyu
- 版本：1.0.0
- 日期：2024-07-31

## 许可证

该项目使用 MIT 许可证，详情请参阅 LICENSE 文件。
