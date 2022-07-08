#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"


 int read_encoder(void); //static
int calc_motor_rotate_speed();
float TSpeed(int arr, int psc );

void TIM3_Int_Init(u16 arr,u16 psc, u8 timer_enable_flag);
void TIM2_Int_Init(u16 arr,u16 psc, u8 timer_enable_flag);  // node0 or node 2(high frequency)
void TIM4_Int_Init(u16 arr,u16 psc); //basic config
void TIM4_PWM_Init(u16 arr,u16 psc); //node1
void TIM4_EncoderMode_Config(void);  //node2
#endif
