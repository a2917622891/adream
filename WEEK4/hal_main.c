#include "stm32f1xx_hal.h"

// 定义LED引脚
#define LED1_PIN    GPIO_PIN_13
#define LED2_PIN    GPIO_PIN_14
#define LED_PORT    GPIOC

// 定义按键引脚
#define EXT_KEY_PIN    GPIO_PIN_0  // PA0 - 外部中断按键
#define FREQ_KEY_PIN   GPIO_PIN_1  // PA1 - 频率切换按键
#define KEY_PORT       GPIOA

// 频率模式枚举
typedef enum {
    FREQ_0_5HZ = 0,  // 0.5Hz (2秒周期)
    FREQ_0_2HZ,      // 0.2Hz (5秒周期)
    FREQ_0_1HZ,      // 0.1Hz (10秒周期)
    FREQ_MODE_COUNT
} FreqMode_t;

volatile uint8_t exti_flag = 0;      // 外部中断标志
volatile uint8_t tim_flag = 0;       // 定时器中断标志
volatile uint8_t exti_processed = 0; // 外部中断已处理标志
volatile uint8_t tim_processed = 0;  // 定时器中断已处理标志
volatile uint8_t frequency_mode = FREQ_0_5HZ; // 当前频率模式

// HAL全局变量
TIM_HandleTypeDef htim3;
uint16_t tim_arr_values[FREQ_MODE_COUNT] = {3999, 9999, 19999}; // 各频率对应的ARR值

// 初始化LED GPIO
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = LED1_PIN | LED2_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);
    
    // 初始状态: LED熄灭
    HAL_GPIO_WritePin(LED_PORT, LED1_PIN | LED2_PIN, GPIO_PIN_SET);
}

// 初始化按键GPIO
void KEY_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // 外部中断按键(PA0)
    GPIO_InitStruct.Pin = EXT_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING; // 下降沿中断
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
    
    // 频率切换按键(PA1)
    GPIO_InitStruct.Pin = FREQ_KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
}

// 配置外部中断
void EXTI_Init(void) {
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

// 初始化定时器3
void TIM3_Init(uint16_t arr) {
    __HAL_RCC_TIM3_CLK_ENABLE();
    
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 36000 - 1;  // 72MHz/36000 = 2KHz
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = arr;  // 自动重载值
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    
    HAL_TIM_Base_Init(&htim3);
    HAL_TIM_Base_Start_IT(&htim3);
    
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

// 重新配置定时器频率
void TIM3_FreqConfig(FreqMode_t mode) {
    // 停止定时器
    HAL_TIM_Base_Stop_IT(&htim3);
    
    // 根据模式设置ARR值
    uint16_t arr_value;
    switch(mode) {
        case FREQ_0_5HZ:
            arr_value = 3999;  // 0.5Hz: 1/(72MHz/36000/4000) = 2s
            break;
        case FREQ_0_2HZ:
            arr_value = 9999;  // 0.2Hz: 1/(72MHz/36000/10000) = 5s
            break;
        case FREQ_0_1HZ:
            arr_value = 19999; // 0.1Hz: 1/(72MHz/36000/20000) = 10s
            break;
        default:
            arr_value = 3999;
    }
    
    // 设置新参数
    __HAL_TIM_SET_AUTORELOAD(&htim3, arr_value);
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    
    // 重新启动定时器
    HAL_TIM_Base_Start_IT(&htim3);
}

// 按键扫描
void KEY_Scan(void) {
    static uint8_t key_state = 1;
    static uint8_t key_last = 1;
    
    // 读取当前按键状态
    uint8_t key_current = HAL_GPIO_ReadPin(KEY_PORT, FREQ_KEY_PIN);
    
    // 按键按下(下降沿)
    if(key_last == 1 && key_current == 0) {
        key_state = 0;
    }
    
    // 按键释放(上升沿)
    if(key_last == 0 && key_current == 1 && key_state == 0) {
        key_state = 1;
        // 切换频率模式
        frequency_mode = (frequency_mode + 1) % FREQ_MODE_COUNT;
        TIM3_FreqConfig(frequency_mode);
    }
    
    key_last = key_current;
}

// 外部中断0回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == EXT_KEY_PIN) {
        // 设置中断标志
        exti_flag = 1;
        exti_processed = 0;
    }
}

// 定时器3更新中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM3) {
        // 设置定时器中断标志
        tim_flag = 1;
        tim_processed = 0;
    }
}

// 系统时钟配置
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    // 使能HSE
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;  // 8MHz * 9 = 72MHz
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
    
    // 配置系统时钟
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // 36MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // 72MHz
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

int main(void) {
    // HAL库初始化
    HAL_Init();
    
    // 配置系统时钟为72MHz
    SystemClock_Config();
    
    // 初始化外设
    LED_Init();
    KEY_Init();
    EXTI_Init();
    TIM3_Init(3999);  // 初始频率0.5Hz
    
    while(1) {
        // 按键扫描
        KEY_Scan();
        
        // 外部中断处理
        if(exti_flag && !exti_processed) {
            // 点亮LED1
            HAL_GPIO_WritePin(LED_PORT, LED1_PIN, GPIO_PIN_RESET);
            
            // 延时300ms(模拟亮灯)
            HAL_Delay(300);
            
            // 熄灭LED1(表示外部中断已处理)
            HAL_GPIO_WritePin(LED_PORT, LED1_PIN, GPIO_PIN_SET);
            
            // 清除标志
            exti_flag = 0;
            exti_processed = 1;
            
            // 简单延时消抖
            HAL_Delay(50);
        }
        
        // 定时器中断处理
        if(tim_flag && !tim_processed) {
            // 点亮LED2
            HAL_GPIO_WritePin(LED_PORT, LED2_PIN, GPIO_PIN_RESET);
            
            // 根据当前频率模式设置亮灯时间
            switch(frequency_mode) {
                case FREQ_0_5HZ:
                    HAL_Delay(400);  // 0.5Hz亮400ms
                    break;
                case FREQ_0_2HZ:
                    HAL_Delay(800);  // 0.2Hz亮800ms
                    break;
                case FREQ_0_1HZ:
                    HAL_Delay(1000); // 0.1Hz亮1000ms
                    break;
            }
            
            // 熄灭LED2(表示定时中断已处理)
            HAL_GPIO_WritePin(LED_PORT, LED2_PIN, GPIO_PIN_SET);
            
            // 清除标志
            tim_flag = 0;
            tim_processed = 1;
        }
    }
}

// 外部中断0中断服务函数
void EXTI0_IRQHandler(void) {
    HAL_GPIO_EXTI_IRQHandler(EXT_KEY_PIN);
}

// 定时器3中断服务函数
void TIM3_IRQHandler(void) {
    HAL_TIM_IRQHandler(&htim3);
}