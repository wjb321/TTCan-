
#include "nodes.h"
#include "can_config.h"
#include "usart.h"
#include "ttcan.h"
#include "stm32f10x.h"
#include "timer.h"
#include "stm32f10x.h"
#include "systick.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>
#include "math.h"
#include "led.h"
#define PI 3.14159

int sm_id_nonzero_flag = 0;
//uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
uint8_t MES1_DATA0,MES1_DATA1,MES1_DATA2,MES1_DATA3,MES1_DATA4,MES1_DATA5,MES1_DATA6,MES1_DATA7;
uint8_t MES2_DATA0,MES2_DATA1,MES2_DATA2,MES2_DATA3,MES2_DATA4,MES2_DATA5,MES2_DATA6,MES2_DATA7;
uint8_t MES3_DATA0,MES3_DATA1,MES3_DATA2,MES3_DATA3,MES3_DATA4,MES3_DATA5,MES3_DATA6,MES3_DATA7;
uint8_t MES4_DATA0,MES4_DATA1,MES4_DATA2,MES4_DATA3,MES4_DATA4,MES4_DATA5,MES4_DATA6,MES4_DATA7;
uint8_t MES5_DATA0,MES5_DATA1,MES5_DATA2,MES5_DATA3,MES5_DATA4,MES5_DATA5,MES5_DATA6,MES5_DATA7;
uint8_t MES6_DATA0,MES6_DATA1,MES6_DATA2,MES6_DATA3,MES6_DATA4,MES6_DATA5,MES6_DATA6,MES6_DATA7;
int MesTimesInBC = 0 ;
uint8_t MasterNodeFlag = 0; ;
uint16_t temp_receive_id, first_mes_after_ref= 0;
uint16_t temp_MesTranTime = 0, temp_sos_ID, interrupt_sos_times = 1, temp= 10000;
uint8_t   SlaveNode1Flag = 0;
uint8_t   SlaveNode2Flag = 0;
/*when receiving the hall speed, get 3 velocity: v1, v2, v3, and compare the diferences*/
float speedArray[2] = {0};
int VeloVar1 = 0, VeloVar2 = 0 ;
// ?????ٶȾ???
float DecelerateRate = 1.5;
float VeloThreshold = 15;
float Phase1 = 0;
float Phase2 = 0;
float Decelerate = 30;
float Radian = 0.523;
float Radius = 0.045;
float intermediate = 0;




void Node0()

{
  CAN__Master_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  Master Node??^_^  *\r\n");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  /*SM sending time*/
  TIM3_Int_Init(1999,7199,ENABLE);//10Khz?ļ???Ƶ?ʣ???????5000Ϊ500ms, 4999 for sm time, 14999 for BC time
  /*BC sending time*/             // 5000 50ms  -> 500 5ms -> 5 0.05ms
  // 5 0.005ms
  TIM2_Int_Init(4999,7199,DISABLE) ; // arr_sm is for sm sending, BC is for Basic cycle sending
  MasterNodeFlag = 1;
  /* Infinite loop */
  while (1)
    {

      if (SM_ID_Sent_flag == ENABLE)
        {
          SendMatrix(MaskID_SM);
          printf("reference message is sent: %d \r\n", SM_ID_Sent_Counter);
          SM_ID_Sent_flag = DISABLE;
        }

      if(mes_send_flag == ENABLE)
        {
          printf("master node sends MasterNumBC =%d:\r\n", MasterNumBC);
          SendRef(MaskID_Ref, TotNumBC, NumSlot, SlotTime);
          mes_send_flag = DISABLE;
          if(MasterNumBC >= TotNumBC)
            {
              MasterNumBC = 0;
            }

        }
    }
}





void Node1()
{
  u16 t;
  u16 len;
  u16 times=0;
  SlaveNode1Flag = 1;
  int Veloarray = 0;
  int FillArrayTimes = 0;
  CAN_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  node 1 gets the reference message");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  TIM4_PWM_Init(899,0);	 //????Ƶ??PWMƵ??=72000000/900=80Khz
  TIM3_Int_Init(3500,7199,DISABLE);//10Khz?ļ???Ƶ?ʣ???????5000Ϊ500ms, 9999: 1 s
  intermediate = Decelerate * Radian * Radius;
  while (1)
    {
      /*get usart interrput(speed info)*/
      if(USART_RX_STA&0x8000)
        {
          len=USART_RX_STA&0x3fff;//?õ??˴ν??յ??????ݳ???
          //printf("\r\n the message that you send is:\r\n\r\n");

          for(t=0; t<len; t++)
            {
              //CAN_DATA0 = USART_RX_BUF[t];
              if(FillArrayTimes < 2)
                {
                  speedArray[Veloarray] = USART_RX_BUF[t] ;
                  if(speedArray[1] < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5) && speedArray[1] < speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);
                      MES4_DATA0 = 1;
                      //printf("%d \r\n", CAN_DATA0);
                      printf("it is blocked \r\n");
                    }
                  else if(speedArray[1] >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 -1.5) && speedArray[1] < speedArray[0]  )
                    {
                      printf("speedArray[1]  is %f\r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);
                      MES4_DATA0 = 2;
                      //  printf("%d \r\n", CAN_DATA0);
                      printf("decelerate \r\n");
                    }
                  else if (speedArray[1] > speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);
                      MES4_DATA0 = 3;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("celerate \r\n");
                    }
                  else if (speedArray[0] == speedArray[1] ) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);
                      printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      // printf("%d \r\n", CAN_DATA0);
                    }
                  else
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);
                      MES4_DATA0 = 5;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("other status\r\n");
                    }
                  FillArrayTimes ++;
                  Veloarray ++;  //1  2 3
                }
              else
                {
                  speedArray[0] = speedArray[1] ;
                  speedArray[1] = (USART_RX_BUF[t]);
                  //Veloarray ++;
                  FillArrayTimes = 3;
                  if(speedArray[1] < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5) && speedArray[1] < speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 1;
                      //printf("%d \r\n", CAN_DATA0);
                      printf("it is blocked \r\n");
                    }
                  else if(speedArray[1] >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 -1.5) && speedArray[1] < speedArray[0]  )
                    {
                      printf("speedArray[1]  is %f\r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 2;
                      //  printf("%d \r\n", CAN_DATA0);
                      printf("decelerate \r\n");
                    }
                  else if (speedArray[1] > speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 3;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("celerate \r\n");
                    }
                  else if (speedArray[0] == speedArray[1] ) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      // printf("%d \r\n", CAN_DATA0);
                    }
                  else
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 5;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("other status\r\n");
                    }

                }
              MES4_DATA1 = USART_RX_BUF[t];
              // USART_SendData(USART1, USART_RX_BUF[t]);//?򴮿?1????????// ??????һ?????ڣ?

              while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//?ȴ????ͽ???
            }
          printf("\r\n\r\n");//???뻻??
          USART_RX_STA=0;
        }
      /*Reiceive SM from master node*/
      if( CanSMFlag == ENABLE )
        {
          LEDC13 = !LEDC13;
          ReiceiveSM();
          NumBC =0;
          CanSMFlag = DISABLE;
        }

      /*Reiceive reference and start BC*/

      if( CanRefFlag == ENABLE )
        {
          LEDC14 = !LEDC14;
          if(NumBC >= TotNumBC)
            {
              NumBC = 0;
            }
          printf("\r\n");
          printf("************ %d. reference start********** \r\n",NumBC);
          printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d, %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);
          printf("***time stamp (accumulateed, new) are (%d, %d)*** \r\n",timeStamp, TimeStampRx1);
          // printf("***time stamp (accumulateed, new) in 0x are (%#x, %#x)*** \r\n",timeStamp, TimeStampRx1);
          // printf("************ %d. reference ends********** \r\n",NumBC);
          MesTimesInBC = TimerISR();
          TimerSlotSet();	// print timer array is:...
          temp_receive_id= Received_mes_id[NumBC][1];//NumBC
          first_mes_after_ref = MesTransmitTime(temp_receive_id);
          //printf("first_mes_after_ref is %d \r\n", first_mes_after_ref);
          //T3 for counting the time slots
          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
            {
              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
            }

          printf("MesTimesInBC is %d \r\n", MesTimesInBC);
          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  //printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);
                  //TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                    }
                  printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
                  printf("++++++++Column =1:for message finish++++++++++ \r\n");
                }
              else
                {
                  TIM3_Int_Init(finalTimerValue[0],7199,ENABLE); // this gets from timerset()
                }
            }
          else
            {
              printf("+++GGG nothing to send ! in NumBc:%d  GGG+++\r\n", NumBC);
            }
          //NodeMesTransmit(Received_mes_id[NumBC][1]);

          interrupt_sos_times =1;
          NumBC++;

          if(NumBC > TotNumBC)   //3>= 3 // -1  //0
            {
              NumBC = 0; // -1;//
            }
          printf("************reference end ********** \r\n");
          printf(" \r\n");
          CanRefFlag = DISABLE;
          finalTimerArrayShift_Nonzero = 1;
          finalTimerArrayShift_zero =1;
          TimerArrayLocation = 0;
        }




      /*SOS triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )
        {

          LEDA0 = !LEDA0;
          printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          //printf("SOS_ID()=: %#x \r\n", temp_sos_ID);
          printf("##########end interrupt########### \r\n");
          printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        {
          u16 led0pwmval=0;
          u8 dir=1;
          LEDA1 = !LEDA1;
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              //LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {
              LEDB8 = !LEDB8; //123
              printf("**ABS INFO??(CANRx_ID, wheel status, speed) = ( %#x ,%d , %d rps, %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1,  Rx1_DATA3);
              while(Rx1_DATA0 == 1)
                {
                  delay_ms(1);
                  if(dir)led0pwmval = led0pwmval+ 10 ;
                  //else led0pwmval = led0pwmval- 10;

                  if(led0pwmval>1500)dir=0;
                  if(led0pwmval==0)dir=1;
                  TIM_SetCompare2(TIM4,led0pwmval); // releasing the brake pad
									printf("pwm works now %d \r\n", led0pwmval);
                }
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }

          CanRxFlag = DISABLE;
        }


    }

}

void Node2()
{
  u16 t;
  u16 len;
  u16 times=0;
  SlaveNode2Flag = 1;
  int FillArrayTimes = 0;
  int Veloarray = 0;
  CAN_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  node 2 gets the reference message");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  TIM3_Int_Init(3500,7199,DISABLE);//10Khz?ļ???Ƶ?ʣ???????5000Ϊ500ms, 9999: 1 s
  TIM4_PWM_Init(899,0);	 //????Ƶ??PWMƵ??=72000000/900=80Khz
  intermediate = Decelerate * Radian * Radius;
  while (1)
    {

      /*get usart interrput(speed info)*/
      if(USART_RX_STA&0x8000)
        {
          len=USART_RX_STA&0x3fff;//?õ??˴ν??յ??????ݳ???
          //printf("\r\n the message that you send is:\r\n\r\n");

          for(t=0; t<len; t++)
            {
              if(FillArrayTimes < 2)
                {
                  speedArray[Veloarray] = (USART_RX_BUF[t]);
                  if(speedArray[1] < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5) && speedArray[1] < speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 1;
                      //printf("%d \r\n", CAN_DATA0);
                      printf("it is blocked \r\n");
                    }
                  else if(speedArray[1] >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 -1.5) && speedArray[1] < speedArray[0]  )
                    {
                      printf("speedArray[1]  is %f\r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 2;
                      //  printf("%d \r\n", CAN_DATA0);
                      printf("decelerate \r\n");
                    }
                  else if (speedArray[1] > speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 3;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("celerate \r\n");
                    }
                  else if (speedArray[0] == speedArray[1] ) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      // printf("%d \r\n", CAN_DATA0);
                    }
                  else
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 5;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("other status\r\n");
                    }
                  FillArrayTimes ++;
                  Veloarray ++;  //1  2 3
                }
              else
                {
                  speedArray[0] = speedArray[1] ;
                  speedArray[1] = (USART_RX_BUF[t]);
                  //Veloarray ++;
                  FillArrayTimes = 3;
                  if(speedArray[1] < ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5) && speedArray[1] < speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 1;
                      //printf("%d \r\n", CAN_DATA0);
                      printf("it is blocked \r\n");
                    }
                  else if(speedArray[1] >= ((speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 -1.5) && speedArray[1] < speedArray[0]  )
                    {
                      printf("speedArray[1]  is %f\r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 2;
                      //  printf("%d \r\n", CAN_DATA0);
                      printf("decelerate \r\n");
                    }
                  else if (speedArray[1] > speedArray[0] )
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 3;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("celerate \r\n");
                    }
                  else if (speedArray[0] == speedArray[1] ) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      // printf("%d \r\n", CAN_DATA0);
                    }
                  else
                    {
                      printf("speedArray[1]  is %f \r\n", speedArray[1] );
                      printf("speedArray[0] is %f \r\n", speedArray[0]);
                      printf("cal is %f \r\n", (speedArray[0] + sqrt(pow(speedArray[0],2) - 4* intermediate))/ 2.0 - 1.5);;
                      MES4_DATA0 = 5;
                      // printf("%d \r\n", CAN_DATA0);
                      printf("other status\r\n");
                    }

                }
              MES4_DATA1 = USART_RX_BUF[t];

              while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//?ȴ????ͽ???
            }
          printf("\r\n\r\n");//???뻻??
          USART_RX_STA=0;
        }
      /*Reiceive SM from master node*/
      if( CanSMFlag == ENABLE )
        {
          LEDC13 = !LEDC13;
          CanSMFlag = DISABLE;
          ReiceiveSM();
          NumBC =0;
        }

      /*Reiceive reference and start BC*/

      if( CanRefFlag == ENABLE )
        {

          LEDC14 = !LEDC14;
          if(NumBC >= TotNumBC)
            {
              NumBC = 0;
            }
          printf("\r\n");
          printf("************ %d. reference start********** \r\n",NumBC);
          printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d, %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);
          printf("***time stamp (accumulateed, new) are (%d, %d)*** \r\n",timeStamp, TimeStampRx1);
          //printf("***time stamp (accumulateed, new) in 0x are (%#x, %#x)*** \r\n",timeStamp, TimeStampRx1);
          //printf("************ %d. reference start********** \r\n",NumBC);
          MesTimesInBC = TimerISR();

          TimerSlotSet();	// print timer array is:...
          temp_receive_id= Received_mes_id[NumBC][1];//NumBC
          first_mes_after_ref = MesTransmitTime(temp_receive_id);
          //T3 for counting the time slots
          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
            {
              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
            }

          printf("MesTimesInBC is %d \r\n", MesTimesInBC);
          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  //printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);

                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                    }
                  //TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                  // printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
                  printf("++++++++Column =1:for message finish++++++++++ \r\n");
                }
              else
                {
                  TIM3_Int_Init(finalTimerValue[0],7199,ENABLE); // this gets from timerset()
                }
            }
          else
            {
              printf("+++GGG nothing to send ! in NumBc:%d  GGG+++\r\n", NumBC);
            }
          interrupt_sos_times =1;
          NumBC++;

          if(NumBC > TotNumBC)   //3>= 3 // -1  //0
            {
              NumBC = 0; // -1;//
            }
          printf("************reference end ********** \r\n");
          printf(" \r\n");
          CanRefFlag = DISABLE;
          finalTimerArrayShift_Nonzero = 1;
          finalTimerArrayShift_zero =1;
          TimerArrayLocation = 0;
        }




      /*SOS triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )
        {

          LEDA0 = !LEDA0;
          printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          printf("##########end interrupt########### \r\n");
          printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        { 
					u16 led0pwmval=0;
          u8 dir=1;
          LEDA1 = !LEDA1;
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              //LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {
              LEDB8 = !LEDB8; //123
              printf("**ABS INFO??(CANRx_ID, wheel status, speed) = ( %#x ,%d , %d rps, %#x)** \r\n", CANRx_ID, Rx1_DATA0, (Rx1_DATA1),  Rx1_DATA3);
              while(Rx1_DATA0 == 1)
                {
                  delay_ms(1);
                  if(dir)led0pwmval = led0pwmval+ 10 ;
                  //else led0pwmval = led0pwmval- 10;

                  if(led0pwmval>1500)dir=0;
                  if(led0pwmval==0)dir=1;
                  TIM_SetCompare2(TIM4,led0pwmval); // releasing the brake pad
									printf("pwm works now %d \r\n", led0pwmval);
                }
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO??(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }

          CanRxFlag = DISABLE;
        }


    }
}



