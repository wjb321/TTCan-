
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
#include "led.h"
extern int tempMesLocation[TotNumBC*NumSlot][2];
extern uint8_t Transmit_time_flag;
extern uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
extern uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5;
extern uint16_t timeStamp,TimeStampRx1;
extern uint16_t Slot_ISR_Timer, mes_id[TotNumBC][NumSlot];
extern uint8_t CanRefFlag, CanSMFlag;
extern uint16_t NumBC, Mes_list[NumMes][MesList];
/*master node*/
extern uint16_t mes_send_flag;
extern uint16_t SM_ID_Sent_flag;
extern uint32_t SM_ID_Sent_Counter;
extern uint8_t  MasterNumBC;
/*slave node 1*/
/*slave node 2*/
extern uint16_t CANRef_ID, CANSM_ID;
extern int i, j;
int sm_id_nonzero_flag = 0;
extern uint16_t  Received_mes_id[TotNumBC][NumSlot];
extern uint8_t received_is_exclusive_window[TotNumBC][NumSlot];
extern uint16_t SM_received_counter; // from isr
extern uint16_t SOS_ISR_flag;
extern uint16_t TimerArray[NumSlot-1];
extern uint16_t finalTimerValue[NumSlot];
extern int finalTimerArrayShift_Nonzero;
extern int finalTimerArrayShift_zero; // 标志最终时间数组中没有从0 开始的情况[0, 300, XXX,...]
extern int TimerArrayLocation;
extern uint8_t CanRxFlag;
extern uint16_t CANRx_ID;
uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
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
int speedArray[3] = {0};
int Veloarray = 0;
int VeloVar1 = 0, VeloVar2 = 0 ;
// 填满速度矩阵
int FillArrayTimes = 0;
float DecelerateRate = 1.5;
float VeloThreshold = 15;

void Node0()
{
  CAN__Master_Configuration();
  printf("\r\n");
  printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
  printf("*  Master Node！^_^  *\r\n");
  printf("*                                                               *\r\n");
  printf("*****************************************************************\r\n");
  /*SM sending time*/
  TIM3_Int_Init(1999,7199,ENABLE);//10Khz的计数频率，计数到5000为500ms, 4999 for sm time, 14999 for BC time
  /*BC sending time*/
  TIM2_Int_Init(4999,7199,DISABLE) ; // arr_sm is for sm sending, BC is for Basic cycle sending // 16 //203 //107  //67
  //12.05.22 test 215 107 35(fastest)  10-34 didnot work
  //500 is 50ms
  //50 is 5 ms, 1/5*10^(-3) = 200hz
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
  CAN_Configuration();
  SlaveNode1Flag = 1;
//  printf("\r\n");
//  printf("*****************************************************************\r\n");
//  printf("*                                                               *\r\n");
//  printf("*  node 1 gets the reference message");
//  printf("*                                                               *\r\n");
//  printf("*****************************************************************\r\n");
  TIM3_Int_Init(3500,7199,DISABLE);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s
  uint8_t Timerlist1flag = 1;
  tempMesLocationInSM LocationArray;
  while (1)
    {
      /*get usart interrput(speed info)*/
      if(USART_RX_STA&0x8000)
        {
          len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
          printf("\r\n the message that you send is:\r\n\r\n");

          for(t=0; t<len; t++)
            {
              //CAN_DATA0 = USART_RX_BUF[t];
              if(FillArrayTimes < 3)
                {
                  speedArray[Veloarray] = USART_RX_BUF[t];
                  FillArrayTimes ++;
                  if(Veloarray == 2)
                    {
                      VeloVar1 = speedArray[0] - speedArray[1] ;
                      VeloVar2 = speedArray[1] - speedArray[2] ;
                      if(VeloVar2 > VeloThreshold  ||VeloVar1 > VeloThreshold)
                        {
                          //printf("it is blocked \r\n");
                          MES4_DATA0 = 1;
                          printf("%d \r\n", CAN_DATA0);
                        }
                      else
                        {
                          //printf("just decelerate \r\n");
                          MES4_DATA0 = 2;
                          printf("%d \r\n", CAN_DATA0);
                        }
                    }
                  Veloarray ++;  //1  2 3
                }
              else
                {
                  speedArray[0] = speedArray[1] ;
                  speedArray[1] = speedArray[2] ;
                  speedArray[2] = USART_RX_BUF[t];
                  //Veloarray ++;
                  FillArrayTimes = 3;
                  VeloVar1 = speedArray[0] - speedArray[1] ;
                  VeloVar2 = speedArray[1] - speedArray[2] ;
                  if(VeloVar2 > VeloThreshold  ||VeloVar1 > VeloThreshold )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 1;
                      printf("%d \r\n", CAN_DATA0);
//                      printf("it is blocked \r\n");
                    }
                  else if( (VeloThreshold > VeloVar2 && VeloVar2 > 3 )||(VeloThreshold > VeloVar1 && VeloVar1 > 3 ) )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 2;
                      printf("%d \r\n", CAN_DATA0);
//                      printf("decelerate \r\n");
                    }
                  else if (VeloVar2 < -2 || VeloVar2 < -2 )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 3;
                      printf("%d \r\n", CAN_DATA0);
                      //printf("celerate \r\n");
                    }
                  else if ( VeloVar2 == 0 ||  VeloVar2 == 0) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      // printf("VeloVar1 is %d \r\n", VeloVar1);
                      // printf("VeloVar2 is %d \r\n", VeloVar2);
                      // printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      printf("%d \r\n", CAN_DATA0);
                    }
                  else
                    {
                      // printf("VeloVar1 is %d \r\n", VeloVar1);
                      // printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 5;
                      printf("%d \r\n", CAN_DATA0);
                      // printf("other status\r\n");
                    }

                }
              MES4_DATA1 = USART_RX_BUF[t];
              // USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据// 发给另一个串口，

              while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
            }
          printf("\r\n\r\n");//插入换行
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
          printf(" \r\n");
          printf("************ %d. reference start********** \r\n",NumBC);
          printf("**ref_info_(data0,data1,data2,data3,data4,data5) = (%d , %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);

          MesTimesInBC = TimerISR();
          TimerSlotSet();	// print timer array is:...
          //  LocationArray = GetLocationInRxSM(tempMesLocation);
          //   for(int hh = 0; hh < TotNumBC; hh++)
          //      {
          //for(int mm = 0; mm < NumSlot; mm++)
          //{

          //         printf("LocationArray[][] is: (%d, %d)\r\n ", LocationArray[hh][0], LocationArray[hh][1]);
          // }
          //    }
          //	int tempLen = sizeof(LocationArray)/ sizeof(LocationArray[0]);  //4;//
          //printf("###tempLen is %d ###\r\n", tempLen);
          //MesTimesInBC = TimesOfBCMes(LocationArray);
          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
            {
              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
            }

          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  // printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  // printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);
                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                    }

                  //  printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
                  // printf("++++++++Column =1:for message finish++++++++++ \r\n");
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
          // printf("************reference end ********** \r\n");
          // printf(" \r\n");
          CanRefFlag = DISABLE;
          finalTimerArrayShift_Nonzero = 1;
          finalTimerArrayShift_zero =1;
          TimerArrayLocation = 0;
        }




      /*SOS(start of slot) triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )
        {

          LEDA0 = !LEDA0;
          printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          //printf("SOS_ID()=: %#x \r\n", temp_sos_ID);
          printf("##########end interrupt########### \r\n");
          //  printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        {
          LEDA1 = !LEDA1;
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {
              LEDB8 = !LEDB8; //123
              printf("**ABS INFO：(CANRx_ID, wheel status, speed) = ( %#x ,%d , %d rps, %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1,  Rx1_DATA3);
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }


          //printf("#$#$#$#$#$#$#$#Reiceive message from CANRx_ID: %#x #$#$#$#$#$#$#$# \r\n", CANRx_ID);
          //printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d rps, %#x, %#x, %#x, %#x, %#x )** \r\n",Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5);
          //printf("++++++++message finish sending++++++++++ \r\n");
          //for(int Veloarray = 0; Veloarray < sizeof(speedArray)/sizeof(speedArray[0]); Veloarray ++)
          //{
          //}
          //	 printf(" \r\n")

          CanRxFlag = DISABLE;
        }


    }

}

void Node2()
{
  u16 t;
  u16 len;
  u16 times=0;
  CAN_Configuration();
  SlaveNode2Flag = 1;
  tempMesLocationInSM LocationArray;
//  printf("\r\n");
//  printf("*****************************************************************\r\n");
//  printf("*                                                               *\r\n");
//  printf("*  node 2 gets the reference message");
//  printf("*                                                               *\r\n");
//  printf("*****************************************************************\r\n");
  TIM3_Int_Init(3500,7199,DISABLE);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s
  uint8_t Timerlist2flag = 1;
  //TimerSlotSet();	// print timer array is:...
  while (1)
    {
      /*usart interrupt about speed*/
      if(USART_RX_STA&0x8000)
        {
          len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
          printf("\r\n the message that you send is:\r\n\r\n");

          for(t=0; t<len; t++)
            {

              //CAN_DATA0 = USART_RX_BUF[t];
              if(FillArrayTimes < 3)
                {
                  speedArray[Veloarray] = USART_RX_BUF[t];
                  FillArrayTimes ++;
                  if(Veloarray == 2)
                    {
                      VeloVar1 = speedArray[0] - speedArray[1] ;
                      VeloVar2 = speedArray[1] - speedArray[2] ;
                      if(VeloVar2 > VeloThreshold  ||VeloVar1 > VeloThreshold)
                        {
                          //printf("it is blocked \r\n");
                          MES4_DATA0 = 1;
                          printf("%d \r\n", MES4_DATA0);
                        }
                      else
                        {
                          //printf("just decelerate \r\n");
                          MES4_DATA0 = 2;
                          printf("%d \r\n", MES4_DATA0);
                        }
                    }
                  Veloarray ++;  //1  2 3
                }
              else
                {
                  speedArray[0] = speedArray[1] ;
                  speedArray[1] = speedArray[2] ;
                  speedArray[2] = USART_RX_BUF[t];
                  //Veloarray ++;
                  FillArrayTimes = 3;
                  VeloVar1 = speedArray[0] - speedArray[1] ;
                  VeloVar2 = speedArray[1] - speedArray[2] ;
                  if(VeloVar2 > VeloThreshold  ||VeloVar1 > VeloThreshold )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 1;
                      printf("%d \r\n", MES4_DATA0);
//                      printf("it is blocked \r\n");
                    }
                  else if( (VeloThreshold > VeloVar2 && VeloVar2 > 3 )||(VeloThreshold > VeloVar1 && VeloVar1 > 3 ) )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 2;
                      printf("%d \r\n", MES4_DATA0);
//                      printf("decelerate \r\n");
                    }
                  else if (VeloVar2 < -2 || VeloVar2 < -2 )
                    {
//                      printf("VeloVar1 is %d \r\n", VeloVar1);
//                      printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 3;
                      printf("%d \r\n", MES4_DATA0);
                      //printf("celerate \r\n");
                    }
                  else if ( VeloVar2 == 0 ||  VeloVar2 == 0) //||  (VeloVar1 >0 && VeloVar2 == 0)
                    {
                      // printf("VeloVar1 is %d \r\n", VeloVar1);
                      // printf("VeloVar2 is %d \r\n", VeloVar2);
                      // printf("constant speed\r\n");
                      MES4_DATA0 = 4;
                      printf("%d \r\n", MES4_DATA0);
                    }
                  else
                    {
                      // printf("VeloVar1 is %d \r\n", VeloVar1);
                      // printf("VeloVar2 is %d \r\n", VeloVar2);
                      MES4_DATA0 = 5;
                      printf("%d \r\n", CAN_DATA0);
                      // printf("other status\r\n");
                    }

                }
              MES4_DATA1 = USART_RX_BUF[t];
              // USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据// 发给另一个串口，

              while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
              //CAN_DATA0 = USART_RX_BUF[t];
              //USART_SendData(USART1, USART_RX_BUF[t]);//向串口1发送数据// 发给另一个串口，
              // printf("%d \r\n", USART_RX_BUF[t]);
              //while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
            }
          printf("\r\n\r\n");//插入换行
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
          // if(Timerlist2flag == 1)
//            {
          MesTimesInBC = TimerISR();
          TimerSlotSet();	// print timer array is:...
//              Timerlist2flag = 0;
//            }
//          for(int wq = 0; wq < TotNumBC; wq++)
//            {
//              for(int wp = 0; wp < NumSlot; wp++)
//                {

//                  printf("Received_mes_id[][] is: %#x\r\n ", Received_mes_id[wq][wp]);
//                }
//            }
          //  LocationArray = GetLocationInRxSM(tempMesLocation);

          //     for(int nm = 0; nm < TotNumBC; nm++)
          //       {
          //for(int mm = 0; mm < NumSlot; mm++)
          //{

          //              printf("LocationArray[][] is: (%d, %d)\r\n ", LocationArray[nm][0], LocationArray[nm][1]);
          // }
          //        }
//int tempLen = sizeof(LocationArray)/ sizeof(LocationArray[0]);  //4;//
//printf("###tempLen is %d ###\r\n", tempLen);

          //   MesTimesInBC = TimesOfBCMes(LocationArray);
          //printf("MesTimesInBC is: %d\r\n ", MesTimesInBC);
//						TimerSlotSet();
          // MesTimesInBC = TimerISR();
          // TimerSlotSet();	// print timer array is:...
          //printf("***time stamp (accumulateed, new) are (%d, %d)*** \r\n",timeStamp, TimeStampRx1);
          //printf("***time stamp (accumulateed, new) in 0x are (%#x, %#x)*** \r\n",timeStamp, TimeStampRx1);
          //  printf("************ %d. reference start********** \r\n",NumBC);

          //temp_receive_id= Received_mes_id[NumBC][1];//NumBC
          //first_mes_after_ref = MesTransmitTime(temp_receive_id);
          //T3 for counting the time slots
          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
            {
              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
            }

//          printf("MesTimesInBC is %d \r\n", MesTimesInBC);
          if(MesTimesInBC !=0)
            {
              if(finalTimerValue[0]== 0)
                {
                  //        printf("++++++++NumBc: %d first message is sent:++++++++++ \r\n", NumBC);
                  // printf("messages should be sent directly after BC comes \r\n");
                  NodeMesTransmit(Received_mes_id[NumBC][1]);
                  if(MesTimesInBC > 1)
                    {
                      TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
                    }
                  //printf("finalTimerValue[finalTimerArrayShift_zero] value: %d++++++++++ \r\n", finalTimerValue[finalTimerArrayShift_zero]);
                  //       printf("++++++++Column =1:for message finish++++++++++ \r\n");
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
          //   printf("************reference end ********** \r\n");
          //   printf(" \r\n");
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
          //    printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;  // final timer array for the timer3 values
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        {
          LEDA1 = !LEDA1;
          //printf("#$#$#$#$#$#$#$#Reiceive message from CANRx_ID: %#x #$#$#$#$#$#$#$# \r\n", CANRx_ID);
          //printf("**receive_(data0,data1,data2,data3,data4,data5) = (%d rps, %#x, %#x, %#x, %#x, %#x )** \r\n",Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5);
          //printf("++++++++message finish sending++++++++++ \r\n");
          //printf(" \r\n");
          if(CANRx_ID == mes1_ID)
            {
              LEDB5 = !LEDB5; // 120
              printf("**Mes1 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes2_ID)
            {
              LEDB6 = !LEDB6; //121
              printf("**Mes2 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes3_ID)
            {
              LEDB7 = !LEDB7; //122
              printf("**Mes3 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes4_ID)
            {
              LEDB8 = !LEDB8; //123
              printf("**ABS INFO：(CANRx_ID, wheel status, speed) = ( %#x ,%d , %d rps, %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1,  Rx1_DATA3);
            }
          else if(CANRx_ID == mes5_ID)
            {
              LEDB9 = !LEDB9; //124
              printf("**Mes5 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else if(CANRx_ID == mes6_ID)
            {
              LEDA2 = !LEDA2; //125
              printf("**Mes6 INFO：(CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3) = ( %#x ,%#x , %#x , %#x)** \r\n", CANRx_ID, Rx1_DATA0, Rx1_DATA1, Rx1_DATA3);
            }
          else
            {
              printf("**arbitrary windows **");
            }
          CanRxFlag = DISABLE;
        }


    }
}



