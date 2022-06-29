#ifndef __ZJJM_H
#define __ZJJM_H	 
#include "sys.h"
#define ENCODER_RESOLUTION 12   /*������һȦ������������*/
#define ENCODER_MULTIPLE 4       /*��������Ƶ��ͨ����ʱ���ı�����ģʽ����*/
#define MOTOR_REDUCTION_RATIO 1 /*����ļ��ٱ�*/
#define TOTAL_RESOLUTION ( ENCODER_RESOLUTION*ENCODER_MULTIPLE*MOTOR_REDUCTION_RATIO ) 
extern int circle_count;
void TIM4_Mode_Config(void);
 int read_encoder(void); //static
void calc_motor_rotate_speed();
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 arr,u16 psc);
#endif
