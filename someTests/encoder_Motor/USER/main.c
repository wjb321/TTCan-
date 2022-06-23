#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "myus.h"
#include "hongw.h"
#include "hcsr.h"
#include "pwm.h"
#include "zjjm.h"
#include "pid.h"
#include "usart.h"

typedef union     //����ṹ�壬�ο�https://blog.csdn.net/ls667/article/details/50811519����
{
	float fdata;
	unsigned long ldata;
}FloatLongType;
void Float_to_Byte(float f,unsigned char byte[]);
extern int circle_count;
extern float v_real;
extern float val;
extern int TIM3_flag;
 int main(void)
 {
	 u8 t_test=0;
	 u8 byte[4]={0};    //��������
	 u8 send_date[4]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  My_LED_Init();
	delay_init();
	TIM4_Mode_Config();
  TIM3_Int_Init(3999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms  
	My_US_Init();
	 GPIO_ResetBits(GPIOC, GPIO_Pin_13);
  while(1)
	{	
//		int value;
//   value =  read_encoder();		
//   printf("%d \r\n ",value);
//		delay_ms(1000);
		//int num=TIM4->CNT;//1024*4=4096  0-4095 
		//	printf("hello the world");
    //printf("%d \r\n",num);
		//printf("%d \r\n",circle_count);
		if(TIM3_flag == 1)
		{
		 //printf("encoder:speed:rps\r\n");
			calc_motor_rotate_speed();
			TIM3_flag = 0;
			
		}
//		//�������ݣ�����ͼ��
//			Float_to_Byte(num*1.0,byte);
//			for(t_test=0;t_test<4;t_test++)
//			{
//				USART_SendData(USART1, byte[t_test]);         //�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}
//			
//			send_date[0]=0X00;send_date[1]=0X00;
//			send_date[2]=0X80;send_date[3]=0X7f;
//			for(t_test=0;t_test<4;t_test++)
//			{
//				USART_SendData(USART1, send_date[t_test]);         //�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}
	}
 }

 
 /****************************************************
��������fת��Ϊ4���ֽ����ݴ����byte[4]��
*****************************************************/
void Float_to_Byte(float f,unsigned char byte[])   //�ο�https://blog.csdn.net/ls667/article/details/50811519����
{
	FloatLongType fl;
	fl.fdata=f;
	byte[0]=(unsigned char)fl.ldata;
	byte[1]=(unsigned char)(fl.ldata>>8);
	byte[2]=(unsigned char)(fl.ldata>>16);
	byte[3]=(unsigned char)(fl.ldata>>24);
}