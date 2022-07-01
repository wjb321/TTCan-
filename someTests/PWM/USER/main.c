#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"


	
 int main(void)
 {		
 	u16 led0pwmval=0;
	u8 dir=1;	
	delay_init();	    	 //��ʱ������ʼ��	  
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
 	TIM4_PWM_Init(899,0);	 //����Ƶ��PWMƵ��=72000000/900=80Khz
  while(1)
	{
 		delay_ms(100);	 
		if(dir)led0pwmval = led0pwmval+ 10 ;
		else led0pwmval = led0pwmval- 10;

 		if(led0pwmval>1500)dir=0;
		if(led0pwmval==0)dir=1;										 
		TIM_SetCompare2(TIM4,led0pwmval);		   
	}	 
 }
