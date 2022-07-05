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

extern u8  TIM5CH1_CAPTURE_STA;		//���벶��״̬		    				
extern u32	TIM5CH1_CAPTURE_VAL;	//���벶��ֵ 

extern int TIM3_flag;

extern u8 isReceiveUartCmd;
int Tmethod = 59;
int main(void)
{ 
	int encoder;
	int t =0;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	delay_init();
	uart_init(115200);//��ʼ�����ڲ�����Ϊ115200
	
	//MOTOT_Init();/*����������ų�ʼ��*/
	TIM2_Int_Init(Tmethod,71);
  TIM4_Mode_Config(ENABLE);//encoder configure
	//TIM3_Int_Init(Tmethod-1,7200-1);/*��ʱ�������Խ����ٶȼ���*/
	/*8400��Ƶ��10kHz, ��1ms����10�Σ�ARR��Ϊ1000����100ms���һ�Σ�ʵ��ÿ100ms����һ��ת��*/

	while(1)
	{
//		if(isReceiveUartCmd)/*���մ��ڵ�PWM����ָ��*/
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
