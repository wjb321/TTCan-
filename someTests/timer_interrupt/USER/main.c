#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "math.h"

int main(void)
{ 
	vu8 key=0;
	int FrequencyInfo = 100;
  KEY_Init();
  delay_init();	    	 //延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  uart_init(115200);	 //串口初始化为115200
  LED_Init();			     //LED端口初始化
  TIM3_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
  delay_us((int)((FrequencyInfo* pow(10,3))/40));
  TIM2_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
  //delay_ms(10);
  while(1)
    {
      key=KEY_Scan(0);	//得到键值
      if(key)
        {
          switch(key)
            {
            case KEY2_PRES:	// speed up
              FrequencyInfo -= 5;
              if (FrequencyInfo <=10)
                {
                  FrequencyInfo = 10;
                }
							printf("speed up %d \r\n", FrequencyInfo);
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;
            case KEY1_PRES:	//slow down
              FrequencyInfo += 5;
              if (FrequencyInfo >=5999)
                {
                  FrequencyInfo = 5999;
                }
								printf("decelerate %d \r\n", FrequencyInfo);
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;
            case KEY0_PRES:	//block
              FrequencyInfo += 10;
              if (FrequencyInfo >=5999)
                {
                  FrequencyInfo = 5999;
                }
							printf("blocked %d \r\n", FrequencyInfo);
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz的计数频率，计数到5000为500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;

//            case WKUP_PRES:	//同时控制LED0,LED1翻转   //WKUP_PRES

//              break;
            }
        }
//		LED0=!LED0;
//		delay_ms(500);
    }


}

