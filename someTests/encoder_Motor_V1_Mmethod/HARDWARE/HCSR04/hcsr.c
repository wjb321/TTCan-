//#include "hcsr.h"
//#include "delay.h"
//#include "pid.h"

//int N=0;
//float distance=0;
//void My_CSB_Init(u16 arr,u16 psc)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//GPIO��ʼ������ʱ����ʼ������ʼ���ⲿ�ж�
//	
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	//PB6-Trig(����)����PB6�����ģʽ PB7-Echo(���)
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//Trig
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//  GPIO_Init(GPIOB, &GPIO_InitStructure);	
//	
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;//Echo
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //��������
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
////���ʱ��tim=(arr+1)*(psc+1)/72000000	50000*72/72000000=500ms
//	TIM_TimeBaseStructure.TIM_Period = arr;
//	TIM_TimeBaseStructure.TIM_Prescaler = psc;
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 
//	
//	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//ʹ���ж�	
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�2��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
//	NVIC_Init(&NVIC_InitStructure); 
//	
//	TIM_Cmd(TIM2, ENABLE);
//}

//float v_real=0;
//int num_now=0;
//float val=0;
//void TIM2_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
//	{
////			num_now=TIM4->CNT;		
////			//6cm*3.14--1024*4*2���� v=num/8192/2*1000*(0.06*3.14)
////			v_real=(num_now-30000)*1.15;//cm/s
////	     TIM4->CNT=30000;	

////			pid.target_val=150;
////			val=PID_realize(v_real);
////			TIM_SetCompare1(TIM3,val*25);


//	}
//    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //����жϱ�־λ
//}

//int count=0;
//float Distance(void)
//{
//	  GPIO_ResetBits(GPIOB, GPIO_Pin_6);//Ԥ������Trig����
//		GPIO_SetBits(GPIOB, GPIO_Pin_6);
//		delay_us(20);
//		GPIO_ResetBits(GPIOB, GPIO_Pin_6);//����10us������
//		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 0);	//�ȴ��źŷ������źŷ�������������0,�źŷ�������Ϊ�ߵ�ƽ��������1
//		//N=0;
//		TIM2->CNT=0;//us�����ȡ
//		while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7) == 1); //�ȴ��źŽ��ܣ��źŷ��������У�����һֱ��λ1 
//		count=TIM2->CNT;
//		//count=N;
//		//��λcm
//		//v = 340m/s = 34000cm/s = 34000cm/10^6us = 0.034cm/us
//		//s = vt/2 = t*0.034/2 = t*0.017 �� t/58	
//		distance=(float)count/58;					
//		return distance;
//}