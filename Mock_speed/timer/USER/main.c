#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "math.h"


extern uint8_t timerSampleFlag;
extern uint8_t timerEncoderFlag;
extern uint8_t timerHighFreqFlag;
float EncoderRiseEdge = 0.0;
float HighFreqRiseEdge = 0.0;
float MTHighFreqRiseEdge = 0.0;
float PulsesPerCylce = 12.0;
float MspeedValue, TspeedValue, MTspeedValue;
float THighFrequence = 1000.0;
float MFrequency_EncoderPulse = 1.0;
float catchPeriod = 4999.0;
int main(void)
{

  delay_init();	    	 //延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  uart_init(115200);	 //串口初始化为115200
  LED_Init();			     //LED端口初始化
  TIM3_Int_Init(((1/MFrequency_EncoderPulse * pow(10, 4)))-1,7199); //99
  TIM4_Int_Init(((1/THighFrequence * pow(10, 4)))-1,7199);  //9
  TIM2_Int_Init(catchPeriod,7199);
  while(1)
    {

      if(timerHighFreqFlag == ENABLE)  //TIM4  //hign freq
        {
          HighFreqRiseEdge ++;     // M2
					MTHighFreqRiseEdge ++;
					timerHighFreqFlag = DISABLE;
          //LED0=!LED0;
        }
//				
				
      if(timerEncoderFlag == ENABLE)  //TIM3   //meida
        {
         LED0=!LED0;
          EncoderRiseEdge ++;  //M1
					TspeedValue = (THighFrequence * 60) / (PulsesPerCylce * HighFreqRiseEdge); // period method
					
					HighFreqRiseEdge = 0;
          
          timerEncoderFlag = DISABLE;

        }


      if(timerSampleFlag == ENABLE) //TIM2   //low
        {
          LED1=!LED1;
          MspeedValue = (EncoderRiseEdge * 60) / (PulsesPerCylce *(( catchPeriod+1)/10) * pow(10.0, -3.0)); //m1
					MTspeedValue = (THighFrequence * 60 * EncoderRiseEdge ) / (PulsesPerCylce * MTHighFreqRiseEdge); // period method
					printf("\r\n");
          printf("Mspeed= %.3f  rpm\r\n", MspeedValue);
					printf("TspeedValue %.3f rpm\r\n", TspeedValue);
					printf("MTspeedValue %.3f rpm\r\n", MTspeedValue);
					printf("\r\n");
          EncoderRiseEdge =0;
          MTHighFreqRiseEdge = 0;
          timerSampleFlag = DISABLE;

        }



    }


}

