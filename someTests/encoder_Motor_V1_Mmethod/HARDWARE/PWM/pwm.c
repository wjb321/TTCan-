#include "pwm.h"
#include "led.h"
//PA6,PA7 TIM3CH12
void My_TIM3_Init(u16 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//��������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//��Ƶ����
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period=arr;//�Զ���װ��ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//Ԥ��Ƶϵ��
	
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OCNPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);
	TIM_OC1Init(TIM3,&TIM_OCInitStruct);//ͨ��1
	TIM_OC2Init(TIM3,&TIM_OCInitStruct);//ͨ��2
	
	TIM_OC1PreloadConfig(TIM3,TIM_OCPreload_Enable);        //ʹ��Ԥװ�ؼĴ���
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);
  	TIM_Cmd(TIM3,ENABLE);        //ʹ��TIM2
	
	//	TIM_SetCompare2(TIM2,1500);//���ñȽ�ֵ
	//	TIM_SetCompare3(TIM2,2000);//���ñȽ�ֵ

		
}
