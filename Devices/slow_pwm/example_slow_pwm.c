#include "button.h"
#include "slow_pwm.h"

BUTTON hbutton1; // 代表 8 个按键

SLOW_PWM hpwm0;
SLOW_PWM hpwm1;

void set_pwm_level(SLOW_PWM *hpwm, uint8_t level)
{
  if (hpwm == &hpwm0)
  {
    if (level)
    {
      HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
    }
  }
  else if (hpwm == &hpwm1)
  {
    if (level)
    {
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    }
    else
    {
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    }
  }
}

// 获取按键状态的函数
uint8_t get_level(BUTTON *hbutton)
{
  uint8_t status = 0;

  if (hbutton == &hbutton1)
  {
    // 方式 1：获取按键状态
    // if (!HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin))
    // {
    //   status |= (1 << 0); // KEY_RIGHT 低电平按下
    // }
    // if (!HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin))
    // {
    //   status |= (1 << 1); // KEY_DOWN 低电平按下
    // }
    // if (!HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin))
    // {
    //   status |= (1 << 2); // KEY_LEFT 低电平按下
    // }
    // if (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin))
    // {
    //   status |= (1 << 3); // KEY_UP 高电平按下
    // }

    // 方式 2：获取按键状态
    // status |= ((~HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) & 0x01) << 0); // KEY_RIGHT 低电平按下
    // status |= ((~HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) & 0x01) << 1);   // KEY_DOWN 低电平按下
    // status |= ((~HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) & 0x01) << 2);   // KEY_LEFT 低电平按下
    // status |= ((HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) & 0x01) << 3);        // KEY_UP 高电平按下

    // 方式 3：获取按键状态
    status |= (!HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) << 0);
    status |= (!HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) << 1);
    status |= (!HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) << 2);
    status |= (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) << 3);

    // 其余位保持为 0
  }

  return status;
}

void button_callback(BUTTON *hbutton, uint8_t pressed, uint8_t released)
{
  if (hbutton == &hbutton1)
  {
    if (pressed == (1 << 0))
    {
      slow_pwm_start(&hpwm0, 20, 100, 0x7f);
      slow_pwm_start(&hpwm1, 100, 50, 10);
      printf("KEY_RIGHT 按下\r\n");
    }
    else if (pressed == (1 << 1))
    {
      slow_pwm_start(&hpwm1, 100, 50, 1);
      printf("KEY_DOWN 按下\r\n");
    }
    else if (pressed == (1 << 2))
    {
      slow_pwm_set_speed(&hpwm0, 100, 20);
      printf("KEY_LEFT 按下\r\n");
    }
    else if (pressed == (1 << 3))
    {
      slow_pwm_set_num(&hpwm0, 0);
      printf("KEY_UP 按下\r\n");
    }

    if (released == (1 << 0))
    {
      printf("KEY_RIGHT 释放\r\n");
    }
    else if (released == (1 << 1))
    {
      slow_pwm_start(&hpwm1, 100, 50, 10);
      printf("KEY_DOWN 释放\r\n");
    }
    else if (released == (1 << 2))
    {
      slow_pwm_set_speed(&hpwm0, 20, 100);
      printf("KEY_LEFT 释放\r\n");
    }
    else if (released == (1 << 3))
    {
      printf("KEY_UP 释放\r\n");
    }
  }
}

void SysTick_Handler(void)
{
  if (HAL_GetTick() % 10 == 0) // 10ms 扫描按键
  {
    button_scan(&hbutton1);
  }

  if (HAL_GetTick() % 5 == 0) // 5ms 扫描 LED
  {
    slow_pwm_ticks(&hpwm0);
    slow_pwm_ticks(&hpwm1);
  }
}

int main(void)
{
  if (button_init(&hbutton1, get_level, button_callback))
  {
    printf("初始化失败\r\n");
  }

  slow_pwm_init(&hpwm0, set_pwm_level);
  slow_pwm_init(&hpwm1, set_pwm_level);

  while (1)
  {
  }
}
