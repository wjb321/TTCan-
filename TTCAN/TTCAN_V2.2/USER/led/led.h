#ifndef LED_H
#define	LED_H
#include "sys.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 
#define LEDC13 PCout(13)// PC13
#define LEDA1 PAout(1)// PC13
#define LEDA0 PAout(0)// PC13
#define LEDC14 PCout(14)// PC13
void led_on_off13(void);
void led_on_off14(void);
//void led_on_off15(void);
void LED_Init(void);//??ʼ??


#define LEDB5 PBout(5)
#define LEDB6 PBout(6)
#define LEDB7 PBout(7)
#define LEDB8 PBout(8)
#define LEDB9 PBout(9)

#define LEDA2 PAout(2)

#endif
