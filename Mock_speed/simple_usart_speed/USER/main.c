#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "math.h"

#define USART_TR_LEN  10
extern uint8_t timerSampleFlag;
extern uint8_t timerEncoderFlag;
extern uint8_t timerHighFreqFlag;
float EncoderRiseEdge = 0.0;
float HighFreqRiseEdge = 11.0;
float MTHighFreqRiseEdge = 0.0;
float PulsesPerCylce = 12.0;
float MspeedValue, TspeedValue = 15.0, MTspeedValue;
uint16_t tran_MspeedValue, tran_TspeedValue, tran_MTspeedValue;
float THighFrequence = 3000.0; //reference frequency
float prescalar = 0;
float MFrequency_EncoderPulse = 1000.0; // car encoder frequency
float catchPeriod = 4999.0; //in ms
int KEY1_PRES_Flag = 0;
int finalTspeedValue = 0;
int main(void)
{

  vu8 key=0;
  KEY_Init();
  delay_init();	    	 //延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  uart_init(256000);	 //串口初始化为115200
  LED_Init();			     //LED端口初始化
  //TIM3_Int_Init(((1/(MFrequency_EncoderPulse  ) * pow(10, 4)))-1,7199, ENABLE); //99
  // TIM4_Int_Init(((1/ (THighFrequence ) * pow(10, 4)))-1,71, ENABLE);  //9
  prescalar = 7200/72;
  TIM2_Int_Init(catchPeriod,7199);
  while(1)
    {
      key=KEY_Scan(0);	//得到键值
      if(key)
        {
          switch(key)
            {
            case KEY2_PRES:	// speed up
              TspeedValue += 1;
              break;
            case KEY1_PRES:	//slow down
              TspeedValue -= 0.1;
              if(TspeedValue<=0){
                  TspeedValue = 0;
                }
              break;
            case KEY0_PRES:	//block
              TspeedValue -= 1;

              break;

            case WKUP_PRES:	//同时控制LED0,LED1翻转   //WKUP_PRES
              catchPeriod -= 19;
              break;
            }
        }//else delay_ms(10);
      if(timerHighFreqFlag == ENABLE)  //TIM4  //hign freq
        {
          HighFreqRiseEdge ++;     // M2
          MTHighFreqRiseEdge ++;
          timerHighFreqFlag = DISABLE;
          //LED0=!LED0;
        }

      if(timerEncoderFlag == ENABLE)  //TIM3   //meida
        {
          LED1=!LED1;
          EncoderRiseEdge ++;  //M1
          timerEncoderFlag = DISABLE;

        }


      if(timerSampleFlag == ENABLE) //TIM2   //low
        {
          LED0=!LED0;
          finalTspeedValue = (int)(TspeedValue*10);
          if(finalTspeedValue>=255)
            {
              finalTspeedValue = 255;
            }
          else if(finalTspeedValue<=0)
            {
              finalTspeedValue = 0;
            }
          printf("%d\n",(uint16_t)finalTspeedValue);
//          USART_SendData(USART1, (uint16_t)(finalTspeedValue));//向串口1发送数据// 发给另一个串口
//          delay_ms(1);
//          USART_SendData(USART1, 0x0d);
//          delay_ms(1);
//          USART_SendData(USART1, 0x0a);
//          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
          EncoderRiseEdge =0;
          MTHighFreqRiseEdge = 0;
          timerSampleFlag = DISABLE;

        }



    }


}

