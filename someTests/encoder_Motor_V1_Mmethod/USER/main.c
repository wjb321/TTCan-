#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "myus.h"
#include "hongw.h"
#include "hcsr.h"
#include "pwm.h"
#include "zjjm.h"
#include "pid.h"
#include "math.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "cmd.h"


//#include "capture.h"
//#include "motor.h"

extern u8  TIM5CH1_CAPTURE_STA;		//输入捕获状态		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值 

extern int TIM3_flag;

extern u8 isReceiveUartCmd;
int Tmethod = 59;
int main(void)
{ 
	int encoder;
	int t =0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	delay_init();
	uart_init(115200);//初始化串口波特率为115200
	
	//MOTOT_Init();/*电机控制引脚初始化*/
	TIM2_Int_Init(Tmethod,71);
  TIM4_Mode_Config(ENABLE);//encoder configure
	//TIM3_Int_Init(Tmethod-1,7200-1);/*定时，周期性进行速度计算*/
	/*8400分频到10kHz, 即1ms计数10次，ARR设为1000，即100ms溢出一次，实现每100ms计算一次转速*/

	while(1)
	{
//		if(isReceiveUartCmd)/*接收串口的PWM调速指令*/
//		{
//			isReceiveUartCmd = 0;
//			process_uart_data();
//		}
		//printf("hello \r\n");
//		if(TIM3_flag ==1)
//		{
//			calc_motor_rotate_speed();
//			TIM3_flag =0;
//		}
	}
}
