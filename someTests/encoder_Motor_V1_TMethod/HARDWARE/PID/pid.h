#ifndef __PID_H
#define __PID_H	 
#include "sys.h"

//#define LED0 PCout(13)

typedef struct 
 {
     float target_val;               		//目标值
     float actual_val;                  //实际值
     float err;                         //定义偏差值
     float err_last;                    //定义上一个偏差值
     float Kp,Ki,Kd;                    //定义比例、积分、微分系数
     float integral;                    //定义积分值
}PID;
 
extern PID pid;

float PID_realize(float temp_val);
void PID_param_init(void);
		 				    
#endif




