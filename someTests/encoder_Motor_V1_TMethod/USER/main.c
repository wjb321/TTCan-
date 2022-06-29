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
#include "math.h"
#define TOTAL_RESOLUTION ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 

//typedef union     //����ṹ�壬�ο�https://blog.csdn.net/ls667/article/details/50811519����
//{
//	float fdata;
//	unsigned long ldata;
//}FloatLongType;
//void Float_to_Byte(float f,unsigned char byte[]);
//extern int circle_count;
//extern float v_real;
//extern float val;
extern int TIM3_flag;
extern u32 NumHighFreq ;
extern int encoderFlag ; 
//extern u32 NumHighFreq;
#define CNT_INIT 0   
 int main(void)
 { 
	  int come = 0;
	  int TimerCNT_Variable[2] = {0}; 
//	 u8 t_test=0;
//	 u8 byte[4]={0};    //��������
//	 u8 send_date[4]={0};
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
 // My_LED_Init();
  LED_Init();
	delay_init();
	TIM4_Mode_Config();//encoder configure
  TIM3_Int_Init(1500,7199);//����T��������ȡһ��ʱ���µı���������������  //2300, 2000 
	TIM2_Int_Init(1,71); // creat a high frequency signal  
	My_US_Init();
	
	//GPIO_ResetBits(GPIOC, GPIO_Pin_13);
  while(1)
	{	
//		int value;
//   value =  read_encoder();		
//   printf("%d \r\n ",value);
//	
	  int num=TIM4->CNT;//1024*4=4096  0-4095 
		//delay_ms(10);
    //printf("%d \r\n",num);
		//printf("%d \r\n",circle_count);
		
//	  int value;
//    value =  read_encoder();	
//		printf("%d \r\n ",value);
//		if(value >=4 )
//		{
//			 printf(" %d \r\n ",NumHighFreq);
//			//NumHighFreq = 0;
//	    	TIM_SetCounter(TIM4, CNT_INIT);/*CNT���ֵ*/
//			TIM_SetCounter(TIM2, CNT_INIT);/*CNT���ֵ*/
//		}
		
		if(num == 4)
		{ 
			int temp = 0;
			u32 tempNumHigh ;
			//printf("num is %d \r\n", num); 
//			printf("%d \r\n", NumHighFreq);
//			if(temp ==0)
//			{
//				TimerCNT_Variable[temp] = NumHighFreq;
//				temp ++;
//			}
//			else if(temp ==1)
//			{
//				TimerCNT_Variable[temp] = NumHighFreq;
//				temp =0 ;
//			}
//			if(TimerCNT_Variable[0]< 5* TimerCNT_Variable[1] ) // ||  )
//			{
//				TimerCNT_Variable[1] = TimerCNT_Variable[0];
//			}
//			else if(TimerCNT_Variable[1]< 5* TimerCNT_Variable[0] )
//			{
//				TimerCNT_Variable[0] = TimerCNT_Variable[1];
//			}
			//printf("%.3f \r\n", pow(10,4)/(TOTAL_RESOLUTION * NumHighFreq ));
			TIM_SetCounter(TIM4, CNT_INIT);/*CNT���ֵ*/
			NumHighFreq = 0;
			
		}

		/*Ȧ��*/
//    if(encoderFlag ==1)
//		{ 
//			
//			come ++;
//			printf("here it comes %d \r\n", come);
//			encoderFlag = 0;
//		}
		
//		if(TIM3_flag == 1)
//		{
//		 //printf("encoder:speed:rps\r\n");
//			calc_motor_rotate_speed();
//			TIM3_flag = 0;
//			
//		}
//		
		
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
//void Float_to_Byte(float f,unsigned char byte[])   //�ο�https://blog.csdn.net/ls667/article/details/50811519����
//{
//	FloatLongType fl;
//	fl.fdata=f;
//	byte[0]=(unsigned char)fl.ldata;
//	byte[1]=(unsigned char)(fl.ldata>>8);
//	byte[2]=(unsigned char)(fl.ldata>>16);
//	byte[3]=(unsigned char)(fl.ldata>>24);
//}