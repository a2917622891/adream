#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "TIM3_PWM.h"
#include "MOTOR.h"
#include "mycontrol.h"
#include "Trace.h"


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	TIM3_PWM_Init(7199,1);   //定时器PWM模式初始化(5KHZ)，用于控制L298N电机调速
	MOTOR_GPIO_Init();       //接L298N引脚IN1到IN4，控制电机正反转
	LED_Init();              //LED灯初始化
	
	
	
	Trace_Init();  //循迹模块初始化
	
	while(1)
	{
		
		Trace_task();   //循迹
		
	}
}

