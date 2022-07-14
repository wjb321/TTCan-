#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
u16 arrValue = 499;
u16 pscValue = 7199;

int main(void)
{
  vu8 key=0;
  int led0pwmval=0;
  u8 dir=1;
  KEY_Init();
  delay_init();	    	 //延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  uart_init(256000);	 //串口初始化为115200
  LED_Init();			     //LED端口初始化
  TIM3_PWM_Init(899,0);	 //不分频。PWM频率=72000000/900=80Khz
	TIM4_EncoderMode_Config();
	TIM2_Int_Init(arrValue,pscValue,ENABLE); 
  while(1)
    {
      key=KEY_Scan(0);	//得到键值
      if(key)
        {
          switch(key)
            {
            case KEY2_PRES: // celerate
							led0pwmval+= 25;
						  if(led0pwmval >=650) led0pwmval =650;
						  //printf("led0pwmval %d \r\n", led0pwmval);
              break;

            case KEY1_PRES:  // decelerate
							led0pwmval-=25;
						 if(led0pwmval <=0) led0pwmval =0;
						//printf("led0pwmval %d \r\n", led0pwmval);
              break;

            case KEY0_PRES:  // high decelerate
							led0pwmval-=50;
						  if(led0pwmval <=0) led0pwmval =0;
						//printf("led0pwmval %d \r\n", led0pwmval);
              break;
						 
						case WKUP_PRES:  // mid high
							led0pwmval=400;
              break;
            }
        }
				TIM_SetCompare2(TIM3,led0pwmval);

    }
}

