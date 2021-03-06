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
float HighFreqRiseEdge = 0.0;
float MTHighFreqRiseEdge = 0.0;
float PulsesPerCylce = 12.0;
uint16_t MspeedValue, TspeedValue, MTspeedValue;
uint16_t tran_MspeedValue, tran_TspeedValue, tran_MTspeedValue;
float THighFrequence = 999.0;
float MFrequency_EncoderPulse = 299.0;
float catchPeriod = 1999.0;
int main(void)
{

  vu8 key=0;
  KEY_Init();
  delay_init();	    	 //延时函数初始化
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  uart_init(256000);	 //串口初始化为115200
  LED_Init();			     //LED端口初始化
  TIM3_Int_Init(((1/MFrequency_EncoderPulse * pow(10, 4)))-1,7199); //99
  TIM4_Int_Init(((1/THighFrequence * pow(10, 4)))-1,7199);  //9
  TIM2_Int_Init(catchPeriod,7199);
  while(1)
    {
      key=KEY_Scan(0);	//得到键值
      if(key)
        {
          switch(key)
            {
            case KEY2_PRES:	// speed up
              //printf("speed up\r\n");
              MFrequency_EncoderPulse += 50;
              if(MFrequency_EncoderPulse >= 18000)
                { 
									MFrequency_EncoderPulse = 18000;
								}
             TIM3_Int_Init(((1/MFrequency_EncoderPulse * pow(10, 4)))-1,7199); //99
              break;
            case KEY1_PRES:	//slow down
              //printf("speed down \r\n");
              MFrequency_EncoderPulse -= 50;
              if(MFrequency_EncoderPulse <= 1)
                {
                 MFrequency_EncoderPulse =1; //99
                }
              TIM3_Int_Init(((1/MFrequency_EncoderPulse * pow(10, 4)))-1,7199); //99
              break;
            case KEY0_PRES:	//block
              //printf("blocked \r\n");
              MFrequency_EncoderPulse = (int) ((MFrequency_EncoderPulse)-200);
              TIM3_Int_Init(((1/MFrequency_EncoderPulse * pow(10, 4)))-1,7199); //99

              break;
            case WKUP_PRES:	//同时控制LED0,LED1翻转   //WKUP_PRES
              catchPeriod += 149;
              TIM2_Int_Init(catchPeriod,7199);
              break;
            }
        }//else delay_ms(10);
//      for(t=0;t<USART_TR_LEN;t++)
//			{
//				USART_SendData(USART1, USART_TX_BUF[t]);//向串口1发送数据// 发给另一个串口
//				delay_ms(20);
//				USART_SendData(USART1, 0x0d);
//				delay_ms(20);
//				USART_SendData(USART1, 0x0a);
//				delay_ms(20);
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//				if(t >= USART_TR_LEN -1)
//				{
//					LED0=!LED0;
//					//delay_ms(500);
//					t = 0;
//				}
//			}
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
          LED1=!LED1;
          EncoderRiseEdge ++;  //M1
          TspeedValue = (THighFrequence ) / (PulsesPerCylce * HighFreqRiseEdge); // period method

          HighFreqRiseEdge = 0;

          timerEncoderFlag = DISABLE;

        }


      if(timerSampleFlag == ENABLE) //TIM2   //low
        {
          LED0=!LED0;
          MspeedValue = (EncoderRiseEdge ) / (PulsesPerCylce *(( catchPeriod+1)/100.0) * pow(10.0, -3.0)); //m1
          MTspeedValue = (uint16_t)((THighFrequence  * EncoderRiseEdge ) / (PulsesPerCylce * MTHighFreqRiseEdge)); // period method
          //printf("(MTspeedValue(both) is %.3f", MTspeedValue);
          //tran_MTspeedValue=decToHex(MTspeedValue,16);
          //printf("MTspeedValue(both) is: %#x \r\n", MTspeedValue);
          //printf("(MTspeedValue(both), tran_MTspeedValue) are:(%#x, %d) \r\n", MTspeedValue, tran_MTspeedValue);
          //printf("(MTspeedValue(both)) are:(%d) \r\n", MTspeedValue);
          //printf("(MTspeedValue(both), MspeedValue(highSpeed), TspeedValue(lowSpeed) ) are:(%#x, %#x, %#x ) \r\n", MTspeedValue, tran_MspeedValue, TspeedValue);
          USART_SendData(USART1, MTspeedValue);//向串口1发送数据// 发给另一个串口
          delay_ms(5);
          USART_SendData(USART1, 0x0d);
          delay_ms(5);
          USART_SendData(USART1, 0x0a);
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
            ;
          //等待发送结束
//					USART_SendData(USART1, MspeedValue);//向串口1发送数据// 发给另一个串口
//					delay_ms(10);
//					USART_SendData(USART1, 0x0d);
//					delay_ms(10);
//					USART_SendData(USART1, 0x0a);
//					USART_SendData(USART1, TspeedValue);//向串口1发送数据// 发给另一个串口
//					delay_ms(10);
//					USART_SendData(USART1, 0x0d);
//					delay_ms(10);
//					USART_SendData(USART1, 0x0a);
          EncoderRiseEdge =0;
          MTHighFreqRiseEdge = 0;
          timerSampleFlag = DISABLE;

        }



    }


}

