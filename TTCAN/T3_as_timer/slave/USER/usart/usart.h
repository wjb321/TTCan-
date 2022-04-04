#ifndef USART_H
#define USART_H
#include "led.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 
void GPIO_Configuration(void);
void USART_Configuration(void);
//void NVIC_Configuration(void);
void NVIC0_Configuration(void);
void NVIC1_Configuration(void);	
#endif