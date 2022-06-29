#ifndef __ZJJM_H
#define __ZJJM_H	 
#include "sys.h"
#define ENCODER_RESOLUTION 12   /*编码器一圈的物理脉冲数*/
#define ENCODER_MULTIPLE 4       /*编码器倍频，通过定时器的编码器模式设置*/
#define MOTOR_REDUCTION_RATIO 1 /*电机的减速比*/
#define TOTAL_RESOLUTION ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 
extern int circle_count;
void TIM4_Mode_Config(void);
 int read_encoder(void); //static
void calc_motor_rotate_speed();
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
#endif
