#include "hongw.h"

void My_HW_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;//����GPIO����Ϣ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//��ʱ������
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_0;//13
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,  &GPIO_InitStruct);
	
}
