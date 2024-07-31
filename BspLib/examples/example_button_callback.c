/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_usb.h"
#include "bsp_log.h"
#include "bsp_button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
BUTTON hbutton0;
BUTTON hbutton1;
BUTTON hbutton2;
BUTTON hbutton3;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t get_level(BUTTON *hbutton)
{
    uint8_t status = 0;
    
    if(hbutton == &hbutton0)
    {
        return ~HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);  // KEY0 低电平
    }
    else if(hbutton == &hbutton1)
    {
        return ~HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);  // KEY1 低电平
    }
    else if(hbutton == &hbutton2)
    {
        return ~HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);  // KEY2 低电平
    }
    else if(hbutton == &hbutton3)
    {
        return HAL_GPIO_ReadPin(WK_UP_GPIO_Port, WK_UP_Pin);  // WK_UP 高电平电平
    }
    
    return status;
}

void buttonCallBack(BUTTON *hbutton, BUTTON_Event button_status)
{
    if(hbutton == &hbutton0)
    {
        if(button_status == PRESS_DOWN)
            printf("KEY0 按下\r\n");
        if(button_status == PRESS_UP)
            printf("KEY0 释放\r\n");
        if(button_status == PRESS_REPEAT)
            printf("KEY0 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton0));
        if(button_status == SINGLE_CLICK)
            printf("KEY0 单击\r\n");
        if(button_status == DOUBLE_CLICK)
            printf("KEY0 双击\r\n");
        if(button_status == LONG_PRESS_START)
            printf("KEY0 长按开始\r\n");
        if(button_status == LONG_PRESS_HOLD)
            printf("KEY0 长按保持\r\n");
    }
    if(hbutton == &hbutton1)
    {
        if(button_status == PRESS_DOWN)
            printf("KEY1 按下\r\n");
        if(button_status == PRESS_UP)
            printf("KEY1 释放\r\n");
        if(button_status == PRESS_REPEAT)
            printf("KEY1 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton1));
        if(button_status == SINGLE_CLICK)
            printf("KEY1 单击\r\n");
        if(button_status == DOUBLE_CLICK)
            printf("KEY1 双击\r\n");
        if(button_status == LONG_PRESS_START)
            printf("KEY1 长按开始\r\n");
        if(button_status == LONG_PRESS_HOLD)
            printf("KEY1 长按保持\r\n");
    }
    if(hbutton == &hbutton2)
    {
        if(button_status == PRESS_DOWN)
            printf("KEY2 按下\r\n");
        if(button_status == PRESS_UP)
            printf("KEY2 释放\r\n");
        if(button_status == PRESS_REPEAT)
            printf("KEY2 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton2));
        if(button_status == SINGLE_CLICK)
            printf("KEY2 单击\r\n");
        if(button_status == DOUBLE_CLICK)
            printf("KEY2 双击\r\n");
        if(button_status == LONG_PRESS_START)
            printf("KEY2 长按开始\r\n");
        if(button_status == LONG_PRESS_HOLD)
            printf("KEY2 长按保持\r\n");
    }
    if(hbutton == &hbutton3)
    {
        if(button_status == PRESS_DOWN)
            printf("KEY3 按下\r\n");
        if(button_status == PRESS_UP)
            printf("KEY3 释放\r\n");
        if(button_status == PRESS_REPEAT)
            printf("KEY3 重复按下，重复次数：%d\r\n", get_button_repeat(&hbutton3));
        if(button_status == SINGLE_CLICK)
            printf("KEY3 单击\r\n");
        if(button_status == DOUBLE_CLICK)
            printf("KEY3 双击\r\n");
        if(button_status == LONG_PRESS_START)
            printf("KEY3 长按开始\r\n");
        if(button_status == LONG_PRESS_HOLD)
            printf("KEY3 长按保持\r\n");
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  static uint8_t count = 0;
  if(htim == &htim6)
  {
    button_ticks();

    if(count >= 100)
    {
        count = 0;
        HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    }
    else
    {
        count++;
    }
  }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  USB_Reset();
  button_init(&hbutton0, get_level, buttonCallBack);
  button_init(&hbutton1, get_level, buttonCallBack);
  button_init(&hbutton2, get_level, buttonCallBack);
  button_init(&hbutton3, get_level, buttonCallBack);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  button_start(&hbutton0);
  button_start(&hbutton1);
  button_start(&hbutton2);
  button_start(&hbutton3);
  HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
