#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "stm32f10x_it.h"


void TIM3_Int_Init(u16 arr_BC , int Timer_enable_flag, u16 psc);
void TIM2_Int_Init(u16 arr_BC , int Timer2_enable_flag , u16 psc);// arr_sm is for sm sending, BC is for Basic cycle sending
#endif
