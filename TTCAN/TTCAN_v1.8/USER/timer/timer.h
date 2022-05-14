#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"



void TIM3_Int_Init(u16 arr,u16 psc, u8 timer_enable_flag);
void TIM2_Int_Init(u16 arr,u16 psc, u8 timer_enable_flag);
#endif
