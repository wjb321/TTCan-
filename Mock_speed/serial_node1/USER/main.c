#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"

#define USART_TR_LEN  10

// first 0x0d then 0x0a
//�ж�������д�����ݡ�while�����ǽ�д�������ͨ������1 ���ͳ�ȥ
 int main(void)
 {	
  float USART_TX_BUF[USART_TR_LEN] = {17.68,22.17,11.54,22.1,23.0,77.0,21.1,34.8,22.7,66};     //���ջ���,���USART_REC_LEN���ֽ�.	 
 	u16 t;  
	u16 len;	
	u16 times=0;
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
 	LED_Init();			     //LED�˿ڳ�ʼ��
	KEY_Init();          //��ʼ���밴�����ӵ�Ӳ���ӿ�
 	while(1)
	{
		for(t=0;t<USART_TR_LEN;t++)
			{
				//USART_SendData(USART1, USART_TX_BUF[t]);//�򴮿�1��������// ������һ������
				printf("the values are: %f", USART_TX_BUF[t]);
				delay_ms(20);
				USART_SendData(USART1, 0x0d);
				delay_ms(20);
				USART_SendData(USART1, 0x0a);
				delay_ms(20);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				if(t >= USART_TR_LEN -1)
				{  
					LED0=!LED0;
					//delay_ms(500);
					t = 0;
				}
			}
	}	

 }

