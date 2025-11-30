#include "stm32f10x.h"
#include "Delay.h"
#include "TIM3_PWM.h"

void MOTOR_GPIO_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}	

void Set_Car_Speed(int Left_speed ,int Right_speed)
{
	//左侧电机
	if(Left_speed>0)
	{
		TIM_SetCompare2(TIM3,Left_speed);  //PA7
		GPIO_SetBits(GPIOB, GPIO_Pin_7);
		GPIO_ResetBits(GPIOB, GPIO_Pin_8);
	}
	else if(Left_speed<0)
	{
		TIM_SetCompare2(TIM3,-Left_speed);  //PA7
		GPIO_ResetBits(GPIOB, GPIO_Pin_7);
		GPIO_SetBits(GPIOB, GPIO_Pin_8);
	}
	else
	{
		TIM_SetCompare2(TIM3,0);            //PA7
		GPIO_SetBits(GPIOB, GPIO_Pin_7);
		GPIO_SetBits(GPIOB, GPIO_Pin_8);
	}
	
	
	//右侧电机
	if(Right_speed>0)
	{
		TIM_SetCompare1(TIM3,Right_speed);  //PA6
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	}
	else if(Right_speed<0)
	{
		TIM_SetCompare1(TIM3,-Right_speed);  //PA6
		GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
	}
	else
	{
		TIM_SetCompare1(TIM3,0);  					//PA6
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
		GPIO_SetBits(GPIOB, GPIO_Pin_6);
	}
	
	
}
