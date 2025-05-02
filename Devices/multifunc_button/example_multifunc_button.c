#include "multifunc_button.h"

BUTTON hbutton0;
BUTTON hbutton1;
BUTTON hbutton2;
BUTTON hbutton3;

uint8_t get_level(BUTTON *hbutton)
{
    uint8_t status = 0;

    if (hbutton == &hbutton0)
    {
        return ~HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin); // KEY_RIGHT 低电平
    }
    else if (hbutton == &hbutton1)
    {
        return ~HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin); // KEY_DOWN 低电平
    }
    else if (hbutton == &hbutton2)
    {
        return ~HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin); // KEY_LEFT 低电平
    }
    else if (hbutton == &hbutton3)
    {
        return HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin); // KEY_UP 高电平
    }

    return status;
}

void buttonCallBack(BUTTON *hbutton, BUTTON_Event button_status)
{
    if (hbutton == &hbutton0)
    {
        if (button_status == PRESS_DOWN)
            printf("KEY_RIGHT 按下\r\n");
        if (button_status == PRESS_UP)
            printf("KEY_RIGHT 释放\r\n");
        if (button_status == PRESS_REPEAT)
            printf("KEY_RIGHT 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton0));
        if (button_status == SINGLE_CLICK)
            printf("KEY_RIGHT 单击\r\n");
        if (button_status == DOUBLE_CLICK)
            printf("KEY_RIGHT 双击\r\n");
        if (button_status == LONG_PRESS_START)
            printf("KEY_RIGHT 长按开始\r\n");
        if (button_status == LONG_PRESS_HOLD)
            printf("KEY_RIGHT 长按保持\r\n");
    }
    if (hbutton == &hbutton1)
    {
        if (button_status == PRESS_DOWN)
            printf("KEY_DOWN 按下\r\n");
        if (button_status == PRESS_UP)
            printf("KEY_DOWN 释放\r\n");
        if (button_status == PRESS_REPEAT)
            printf("KEY_DOWN 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton1));
        if (button_status == SINGLE_CLICK)
            printf("KEY_DOWN 单击\r\n");
        if (button_status == DOUBLE_CLICK)
            printf("KEY_DOWN 双击\r\n");
        if (button_status == LONG_PRESS_START)
            printf("KEY_DOWN 长按开始\r\n");
        if (button_status == LONG_PRESS_HOLD)
            printf("KEY_DOWN 长按保持\r\n");
    }
    if (hbutton == &hbutton2)
    {
        if (button_status == PRESS_DOWN)
            printf("KEY_LEFT 按下\r\n");
        if (button_status == PRESS_UP)
            printf("KEY_LEFT 释放\r\n");
        if (button_status == PRESS_REPEAT)
            printf("KEY_LEFT 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton2));
        if (button_status == SINGLE_CLICK)
            printf("KEY_LEFT 单击\r\n");
        if (button_status == DOUBLE_CLICK)
            printf("KEY_LEFT 双击\r\n");
        if (button_status == LONG_PRESS_START)
            printf("KEY_LEFT 长按开始\r\n");
        if (button_status == LONG_PRESS_HOLD)
            printf("KEY_LEFT 长按保持\r\n");
    }
    if (hbutton == &hbutton3)
    {
        if (button_status == PRESS_DOWN)
            printf("KEY_UP 按下\r\n");
        if (button_status == PRESS_UP)
            printf("KEY_UP 释放\r\n");
        if (button_status == PRESS_REPEAT)
            printf("KEY_UP 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton3));
        if (button_status == SINGLE_CLICK)
            printf("KEY_UP 单击\r\n");
        if (button_status == DOUBLE_CLICK)
            printf("KEY_UP 双击\r\n");
        if (button_status == LONG_PRESS_START)
            printf("KEY_UP 长按开始\r\n");
        if (button_status == LONG_PRESS_HOLD)
            printf("KEY_UP 长按保持\r\n");
    }
}

// 定时器回调函数
// 该函数会在定时器中断中被调用，用于定时扫描按钮状态
void TIM_Callback()
{
    button_ticks(); // 扫描按钮状态
}

int main(void)
{
    button_init(&hbutton0, get_level, buttonCallBack);
    button_init(&hbutton1, get_level, buttonCallBack);
    button_init(&hbutton2, get_level, buttonCallBack);
    button_init(&hbutton3, get_level, buttonCallBack);

    button_start(&hbutton0);
    button_start(&hbutton1);
    button_start(&hbutton2);
    button_start(&hbutton3);

    while (1)
    {
    }
}
