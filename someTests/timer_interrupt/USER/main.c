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
  delay_init();	    	 //��ʱ������ʼ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
  LED_Init();			     //LED�˿ڳ�ʼ��
  TIM3_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
  delay_us((int)((FrequencyInfo* pow(10,3))/40));
  TIM2_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
  //delay_ms(10);
  while(1)
    {
      key=KEY_Scan(0);	//�õ���ֵ
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
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;
            case KEY1_PRES:	//slow down
              FrequencyInfo += 5;
              if (FrequencyInfo >=5999)
                {
                  FrequencyInfo = 5999;
                }
								printf("decelerate %d \r\n", FrequencyInfo);
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;
            case KEY0_PRES:	//block
              FrequencyInfo += 10;
              if (FrequencyInfo >=5999)
                {
                  FrequencyInfo = 5999;
                }
							printf("blocked %d \r\n", FrequencyInfo);
              TIM3_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
              delay_us((int)((FrequencyInfo* pow(10,3))/40));
              TIM2_Int_Init(FrequencyInfo,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
								printf("delay time: %.3f , int delay time %d\r\n", ((FrequencyInfo* pow(10,3))/40.0), (int)((FrequencyInfo* pow(10,3))/40) );
              break;

//            case WKUP_PRES:	//ͬʱ����LED0,LED1��ת   //WKUP_PRES

//              break;
            }
        }
//		LED0=!LED0;
//		delay_ms(500);
    }


}

