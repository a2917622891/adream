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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 仅两种模式，简洁明了
typedef enum {
    MODE_TOGGLE,  // 按A翻转LED
    MODE_HOLD     // 按A亮、松A灭
} KeyMode;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 引脚定义
#define LED_Pin    GPIO_PIN_0
#define LED_Port   GPIOB
#define KEYA_Pin   GPIO_PIN_0
#define KEYA_Port  GPIOA
#define KEYB_Pin   GPIO_PIN_1
#define KEYB_Port  GPIOA

#define DEBOUNCE 20  // 消抖时间
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
KeyMode now_mode = MODE_TOGGLE;  // 默认模式：翻转
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
// 简单按键读取
uint8_t Key_Press(GPIO_TypeDef *port, uint16_t pin);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 按键读取函数
uint8_t Key_Press(GPIO_TypeDef *port, uint16_t pin)
{
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)  // 检测按键按下（低电平）
    {
        HAL_Delay(DEBOUNCE);  // 消抖延时
        if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_RESET)  // 再次确认按下
        {
            return 1;
        }
    }
    return 0;
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

  /* MCU初始化：复位外设、初始化Flash和SysTick */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* 初始化所有配置的外设 */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  // 上电强制LED灭
  HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* 主循环 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // 1. 按键B：切换模式
    if (Key_Press(KEYB_Port, KEYB_Pin) == 1)
    {
        now_mode = (now_mode == MODE_TOGGLE) ? MODE_HOLD : MODE_TOGGLE;  // 切换模式
        HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);  // 切换后强制灭LED
        while (Key_Press(KEYB_Port, KEYB_Pin) == 1);  // 等待按键松开，防连按
    }

    // 2. 模式1：按A翻转LED
    if (now_mode == MODE_TOGGLE)
    {
        if (Key_Press(KEYA_Port, KEYA_Pin) == 1)
        {
            // 翻转LED电平（SET=灭，RESET=亮）
            HAL_GPIO_TogglePin(LED_Port, LED_Pin);
            while (Key_Press(KEYA_Port, KEYA_Pin) == 1);  // 等待松开
        }
    }

    // 3. 模式2：按A亮、松A灭
    if (now_mode == MODE_HOLD)
    {
        if (Key_Press(KEYA_Port, KEYA_Pin) == 1)
        {
            HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_RESET);  // 按住亮
        }
        else
        {
            HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);    // 松开灭
        }
    }

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

  /** 初始化RCC振荡器 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 初始化CPU、AHB和APB总线时钟 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO端口时钟使能 */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 配置LED引脚初始电平：高电平（灭） */
  HAL_GPIO_WritePin(LED_Port, LED_Pin, GPIO_PIN_SET);

  /* 配置LED引脚（推挽输出） */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_Port, &GPIO_InitStruct);

  /* 配置按键A和B（上拉输入） */
  GPIO_InitStruct.Pin = KEYA_Pin|KEYB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(KEYA_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  错误处理函数
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  报告断言失败的文件名和行号
  * @param  file: 文件名指针
  * @param  line: 错误行号
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 可添加自定义报告逻辑，如打印信息 */
  /* USER CODE END 6 */
}
#endif
