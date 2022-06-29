#ifndef USART_H
#define USART_H
#include "led.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 
#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	


void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC0_Configuration(void);
void NVIC1_Configuration(void);	
#endif
