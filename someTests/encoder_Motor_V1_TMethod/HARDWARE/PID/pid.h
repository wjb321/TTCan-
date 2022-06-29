#ifndef __PID_H
#define __PID_H	 
#include "sys.h"

//#define LED0 PCout(13)

typedef struct 
 {
     float target_val;               		//Ŀ��ֵ
     float actual_val;                  //ʵ��ֵ
     float err;                         //����ƫ��ֵ
     float err_last;                    //������һ��ƫ��ֵ
     float Kp,Ki,Kd;                    //������������֡�΢��ϵ��
     float integral;                    //�������ֵ
}PID;
 
extern PID pid;

float PID_realize(float temp_val);
void PID_param_init(void);
		 				    
#endif




