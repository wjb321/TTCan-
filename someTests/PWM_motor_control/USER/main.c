#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
u16 arrValue = 99;
u16 pscValue = 71;
u16 Pulse = 128;
int main(void)
{ 

  vu8 key=0;
  int led0pwmval=60;
  u8 dir=1;
  KEY_Init();
  delay_init();	    	 //��ʱ������ʼ��
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  uart_init(256000);	 //���ڳ�ʼ��Ϊ115200
  LED_Init();			     //LED�˿ڳ�ʼ��
	TIM2_Int_Init(arrValue,pscValue,ENABLE); 
  TIM3_PWM_Init(899,0);	 //����Ƶ��PWMƵ��=72000000/900=80Khz
	TIM4_EncoderMode_Config(Pulse);
	
  while(1)
    {
      key=KEY_Scan(0);	//�õ���ֵ
      if(key)
        {
          switch(key)
            {
            case KEY2_PRES: // decelerate
							led0pwmval+= 5;
						  if(led0pwmval >=650) led0pwmval =650;
						  printf("led0pwmval %d \r\n", led0pwmval);
              break;

            case KEY1_PRES:  // decelerate
							led0pwmval +=15;
						 if(led0pwmval >=650) led0pwmval =650;
						  printf("led0pwmval %d \r\n", led0pwmval);
              break;

            case KEY0_PRES:  // celerate
							led0pwmval -=5;
						  if(led0pwmval <=60) led0pwmval =60;
						  printf("led0pwmval %d \r\n", led0pwmval);
              break;
						 
						case WKUP_PRES:  // mid high
							led0pwmval -=20;
						  if(led0pwmval <=60) led0pwmval =60;
						 printf("led0pwmval %d \r\n", led0pwmval);
              break;
            }
        }
				TIM_SetCompare2(TIM3,led0pwmval);

    }
}

