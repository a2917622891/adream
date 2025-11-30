#include "stm32f10x.h"
#include "Delay.h"
#include "Trace.h"
#include "MOTOR.h"




//寻迹任务
void Trace_task(void)
{

	if(X1==0&&X3==0) Set_Car_Speed(4800,4800);
		
	if(X1==1&&X3==0) Set_Car_Speed(4200,0);   //右转
		
	if(X1==0&&X3==1) Set_Car_Speed(0,4200);
	
	if(X2==1&&X1==1&&X3==1&&X4==0) Set_Car_Speed(5000,0);
		
	if(X2==0&&X1==1&&X3==1&&X4==1) Set_Car_Speed(0,5000);
		
	
	//拐右直角
	if(X3==0&&X4==0)
	{
		Set_Car_Speed(0,0);  //停车一下
		Delay_ms(500);
		do
		{
			Set_Car_Speed(5200,-5200);  //原地右转
		}while(X1==0);
	}
	
	//拐左直角
	if(X2==0&&X1==0)
	{
		Set_Car_Speed(0,0);  //停车一下
		Delay_ms(500);
		do
		{
			Set_Car_Speed(-5200,5200);  //原地左转
		}while(X3==0);
	}
	
	

}

