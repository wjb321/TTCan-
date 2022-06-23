#ifndef __ZJJM_H
#define __ZJJM_H	 
#include "sys.h"
extern int circle_count;
void TIM4_Mode_Config(void);
 int read_encoder(void); //static
void calc_motor_rotate_speed();
void TIM3_Int_Init(u16 arr,u16 psc);
#endif
