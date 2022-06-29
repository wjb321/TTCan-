#include "pid.h"
PID pid;
void PID_param_init(void)
 {
     /* ��ʼ������ */
     pid.target_val=0.0;
     pid.actual_val=0.0;
     pid.err=0.0;
     pid.err_last=0.0;
     pid.integral=0.0;
     pid.Kp = 1.0;
     pid.Ki = 0.0;
     pid.Kd = -0.4;
 }
 
float PID_realize(float temp_val)
{
     /*����Ŀ��ֵ��ʵ��ֵ�����*/
    pid.err=pid.target_val-temp_val;
     /*����ۻ�*/
    pid.integral+=pid.err;
     /*PID�㷨ʵ��*/
    pid.actual_val=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
     /*����*/
    pid.err_last=pid.err;
     /*���ص�ǰʵ��ֵ*/
    return pid.actual_val;
}