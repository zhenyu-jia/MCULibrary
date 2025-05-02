#include <stdio.h>
#include "button.h"

BUTTON hbutton1; // 代表 8 个按键

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
    // status |= (!HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) << 0);
    // status |= (!HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) << 1);
    // status |= (!HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) << 2);
    // status |= (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) << 3);

    // 其余位保持为 0
  }

  return status;
}

// 按钮回调函数
void button_callback(BUTTON *hbutton, uint8_t pressed, uint8_t released)
{
  if (hbutton == &hbutton1)
  {
    if (pressed == (1 << 0))
      printf("KEY_RIGHT 按下\r\n");
    if (released == (1 << 0))
      printf("KEY_RIGHT 释放\r\n");

    if (pressed == (1 << 1))
      printf("KEY_DOWN 按下\r\n");
    if (released == (1 << 1))
      printf("KEY_DOWN 释放\r\n");

    if (pressed == (1 << 2))
      printf("KEY_LEFT 按下\r\n");
    if (released == (1 << 2))
      printf("KEY_LEFT 释放\r\n");

    if (pressed == (1 << 3))
      printf("KEY_UP 按下\r\n");
    if (released == (1 << 3))
      printf("KEY_UP 释放\r\n");
  }
}

// 组合按键回调函数
void combination_button_callback(BUTTON *hbutton, uint8_t index)
{
  if (hbutton == &hbutton1)
  {
    printf("组合按键 %d 触发\r\n", index);
  }
}

// 定时器回调函数
// 该函数会在定时器中断中被调用，用于定时扫描按钮状态
void TIM_Callback()
{
  button_scan(&hbutton1);
}

int main(void)
{
  if (button_init(&hbutton1, get_level, button_callback))
  {
    printf("初始化失败\r\n");
  }
#ifdef BUTTON_COMBINATION // 如果启用组合按键功能

  if (button_enable_combination(&hbutton1, 0, 0x01, combination_button_callback) < 0) // 0b00000001
  {
    printf("组合按键 %d 初始化失败\r\n", 0);
  }
  if (button_enable_combination(&hbutton1, 1, 0x02, combination_button_callback) < 0) // 0b00000010
  {
    printf("组合按键 %d 初始化失败\r\n", 1);
  }
  if (button_enable_combination(&hbutton1, 2, 0x04, combination_button_callback) < 0) // 0b00000100
  {
    printf("组合按键 %d 初始化失败\r\n", 2);
  }
  if (button_enable_combination(&hbutton1, 3, 0x08, combination_button_callback) < 0) // 0b000001000
  {
    printf("组合按键 %d 初始化失败\r\n", 3);
  }
  if (button_enable_combination(&hbutton1, 4, 0x03, combination_button_callback) < 0) // 0b00000011
  {
    printf("组合按键 %d 初始化失败\r\n", 4);
  }
  if (button_enable_combination(&hbutton1, 5, 0x05, combination_button_callback) < 0) // 0b00000101
  {
    printf("组合按键 %d 初始化失败\r\n", 5);
  }
  if (button_enable_combination(&hbutton1, 6, 0x09, combination_button_callback) < 0) // 0b00001001
  {
    printf("组合按键 %d 初始化失败\r\n", 6);
  }
  if (button_enable_combination(&hbutton1, 7, 0x06, combination_button_callback) < 0) // 0b00000110
  {
    printf("组合按键 %d 初始化失败\r\n", 7);
  }
  if (button_enable_combination(&hbutton1, 8, 0x0A, combination_button_callback) < 0) // 0b00001010
  {
    printf("组合按键 %d 初始化失败\r\n", 8);
  }
  if (button_enable_combination(&hbutton1, 9, 0x0C, combination_button_callback) < 0) // 0b00001100
  {
    printf("组合按键 %d 初始化失败\r\n", 9);
  }
  if (button_enable_combination(&hbutton1, 10, 0x07, combination_button_callback) < 0) // 0b00000111
  {
    printf("组合按键 %d 初始化失败\r\n", 10);
  }
  if (button_enable_combination(&hbutton1, 11, 0x0B, combination_button_callback) < 0) // 0b00001011
  {
    printf("组合按键 %d 初始化失败\r\n", 11);
  }
  if (button_enable_combination(&hbutton1, 12, 0x0D, combination_button_callback) < 0) // 0b00001101
  {
    printf("组合按键 %d 初始化失败\r\n", 12);
  }
  if (button_enable_combination(&hbutton1, 13, 0x0E, combination_button_callback) < 0) // 0b00001110
  {
    printf("组合按键 %d 初始化失败\r\n", 13);
  }
  if (button_enable_combination(&hbutton1, 14, 0x0F, combination_button_callback) < 0) // 0b00001111
  {
    printf("组合按键 %d 初始化失败\r\n", 14);
  }

#endif // 组合按键功能

  // while (1)
  // {
  // }
}
