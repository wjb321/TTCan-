#include "pid.h"
PID pid;
void PID_param_init(void)
 {
     /* 初始化参数 */
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
     /*计算目标值与实际值的误差*/
    pid.err=pid.target_val-temp_val;
     /*误差累积*/
    pid.integral+=pid.err;
     /*PID算法实现*/
    pid.actual_val=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
     /*误差传递*/
    pid.err_last=pid.err;
     /*返回当前实际值*/
    return pid.actual_val;
}