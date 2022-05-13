#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"

#define USART_TR_LEN  10

// first 0x0d then 0x0a
//中断里面是写入数据。while里面是将写入的数据通过串口1 发送出去
 int main(void)
 {	
  float USART_TX_BUF[USART_TR_LEN] = {17.68,22.17,11.54,22.1,23.0,77.0,21.1,34.8,22.7,66};     //接收缓冲,最大USART_REC_LEN个字节.	 
 	u16 t;  
	u16 len;	
	u16 times=0;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(115200);	 //串口初始化为115200
 	LED_Init();			     //LED端口初始化
	KEY_Init();          //初始化与按键连接的硬件接口
 	while(1)
	{
		for(t=0;t<USART_TR_LEN;t++)
			{
				//USART_SendData(USART1, USART_TX_BUF[t]);//向串口1发送数据// 发给另一个串口
				printf("the values are: %f", USART_TX_BUF[t]);
				delay_ms(20);
				USART_SendData(USART1, 0x0d);
				delay_ms(20);
				USART_SendData(USART1, 0x0a);
				delay_ms(20);
				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				if(t >= USART_TR_LEN -1)
				{  
					LED0=!LED0;
					//delay_ms(500);
					t = 0;
				}
			}
	}	

 }

