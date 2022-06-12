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
 TIM2_Int_Init(catchPeriod ,7199);
  while(1)
    {
      key=KEY_Scan(0);	//得到键值
      if(key)
        {
          switch(key)
            {
            
						
						case KEY2_PRES:	// speed up
							
              //printf("speed up\r\n");
//              MFrequency_EncoderPulse += 60;
//              if(MFrequency_EncoderPulse >= 4900)
//                { 
//									MFrequency_EncoderPulse = 4900;
//								}
          //   TIM3_Int_Init(((1/(MFrequency_EncoderPulse  ) * pow(10, 4)))-1,7199, DISABLE); //99
				//		TIM4_Int_Init(((1/ (THighFrequence ) * pow(10, 4)))-1,71, DISABLE);  //9
						 TspeedValue += 1;
		
              break;
            
						
						case KEY1_PRES:	//slow down
//							KEY1_PRES_Flag = 1;
//              //printf("speed down \r\n");
//              MFrequency_EncoderPulse -= 40;
//              if(MFrequency_EncoderPulse <= 1)
//                {
//                 MFrequency_EncoderPulse =1; //99
//                }
          //  TIM3_Int_Init(((1/(MFrequency_EncoderPulse) * pow(10, 4))) -1,7199,DISABLE); //99
					//	TIM4_Int_Init(((1/ (THighFrequence ) * pow(10, 4)))-1,71, DISABLE);  //9
					  TspeedValue -= 0.1;
					  if(TspeedValue<=0)
						{
							TspeedValue = 0;
						}
								//printf("MFrequency_EncoderPulse(lowSpeed) ) is: %.4f ) \r\n",MFrequency_EncoderPulse);
							//	printf("ms valuse ) is: %.4f ) \r\n",((1/(MFrequency_EncoderPulse) * pow(10, 4))) -1);
              break;
           

						case KEY0_PRES:	//block
              //printf("blocked \r\n");
//              MFrequency_EncoderPulse -=70;
//						              if(MFrequency_EncoderPulse <= 1)
//                {
//                 MFrequency_EncoderPulse =1; //99
//                }
           //   TIM3_Int_Init(((1/(MFrequency_EncoderPulse ) * pow(10, 4))) -1,7199,DISABLE); //99
					//	 TIM4_Int_Init(((1/ (THighFrequence ) * pow(10, 4)))-1,71, DISABLE);  //9
              TspeedValue -= 1;

              break;
           
						case WKUP_PRES:	//同时控制LED0,LED1翻转   //WKUP_PRES
              catchPeriod -= 19;
        //      TIM2_Int_Init(catchPeriod,7199);
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
;
          //TspeedValue = ((THighFrequence * prescalar) / (PulsesPerCylce * HighFreqRiseEdge)); // period method (uint16_t) (int)
//					printf("MFrequency_EncoderPulse(lowSpeed) ) is: %.2f \r\n",MFrequency_EncoderPulse);
//           printf("TspeedValue(lowSpeed) ) is: %d \r\n",TspeedValue);
//					 USART_SendData(USART1, TspeedValue);//向串口1发送数据// 发给另一个串口
//         delay_ms(1);
//          USART_SendData(USART1, 0x0d);
//          delay_ms(1);
//          USART_SendData(USART1, 0x0a);
//					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
//													printf("\r\n");
//					printf("HighFreqRiseEdge ) is: %.3f ) \r\n",HighFreqRiseEdge);
//					printf("\r\n");

          //HighFreqRiseEdge = 0;

          timerEncoderFlag = DISABLE;

        }


      if(timerSampleFlag == ENABLE) //TIM2   //low
        {
          LED0=!LED0;
//          MspeedValue = (EncoderRiseEdge ) / (PulsesPerCylce *(( catchPeriod+1)/100.0) * pow(10.0, -3.0)); //m1
//          MTspeedValue = (uint16_t)((THighFrequence  * EncoderRiseEdge ) / (PulsesPerCylce * MTHighFreqRiseEdge)); // period method
         // TspeedValue = ((THighFrequence * prescalar) / (PulsesPerCylce * HighFreqRiseEdge)); // period method (uint16_t) (int)
//										printf("MFrequency_EncoderPulse(lowSpeed) ) is: %.2f \r\n",MFrequency_EncoderPulse);
          //printf("TspeedValue(lowSpeed) ) is: %.4f \r\n",TspeedValue);
          //printf("(MTspeedValue(both) is %d", MTspeedValue);
          //tran_MTspeedValue=decToHex(MTspeedValue,16);
          //printf("MTspeedValue(both) is: %#x \r\n", MTspeedValue);
         // printf("(MTspeedValue(both), tran_MTspeedValue) are:(%d, %d) \r\n", MTspeedValue, tran_MTspeedValue);
         // printf("(MTspeedValue(both)) are:(%d) \r\n", MTspeedValue);
          //USART_SendData(USART1, MTspeedValue);//向串口1发送数据// 发给另一个串口
         // delay_ms(5);
          //USART_SendData(USART1, 0x0d);
          //delay_ms(5);
          //USART_SendData(USART1, 0x0a);
         // while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
            ;
          //等待发送结束
//					USART_SendData(USART1, MspeedValue);//向串口1发送数据// 发给另一个串口
//					delay_ms(10);
//					USART_SendData(USART1, 0x0d);
//					delay_ms(10);
//					USART_SendData(USART1, 0x0a);

          finalTspeedValue = (int)(TspeedValue*10);
					if(finalTspeedValue>=255)
						{
							finalTspeedValue = 255;
						}
					else if(finalTspeedValue<=0)
						{
							finalTspeedValue = 0;
						}
        //printf("TspeedValue(lowSpeed) ) is: %d ) \r\n",(uint16_t)finalTspeedValue);
					USART_SendData(USART1, (uint16_t)(finalTspeedValue));//向串口1发送数据// 发给另一个串口
					delay_ms(5);
					USART_SendData(USART1, 0x0d);
					delay_ms(5);
					USART_SendData(USART1, 0x0a);
          while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET)
          EncoderRiseEdge =0;
          MTHighFreqRiseEdge = 0;
          timerSampleFlag = DISABLE;

        }



    }


}

