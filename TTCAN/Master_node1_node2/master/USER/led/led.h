#ifndef LED_H
#define	LED_H
#include "sys.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 
#define LED0 PCout(13)// PC13

void led_on_off(void);

void LED_Init(void);//≥ı ºªØ

#endif
