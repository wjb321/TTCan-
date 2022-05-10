
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
int MesTimesInBC = 0 ;
uint8_t MasterNodeFlag = 0; ;
uint16_t temp_receive_id, first_mes_after_ref= 0;
uint16_t temp_MesTranTime = 0, temp_sos_ID, interrupt_sos_times = 1, temp= 10000;
uint8_t   SlaveNode1Flag = 0;
uint8_t   SlaveNode2Flag = 0;

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
  TIM2_Int_Init(8000,7199,DISABLE) ; // arr_sm is for sm sending, BC is for Basic cycle sending // 16 //203 //107  //67
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
          printf("************ %d. reference start********** \r\n",NumBC);
          printf("**ref_info_(data0,data1,data2,data3,data4,data5) = (%d, %d, %d, %d, %d, %d )** \r\n",Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5);

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




      /*SOS triggered internal messages*/
      if(SOS_ISR_flag == ENABLE )
        {

          LEDA0 = !LEDA0;
          // printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          //printf("SOS_ID()=: %#x \r\n", temp_sos_ID);
          //  printf("##########end interrupt########### \r\n");
          //  printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        {
          LEDA1 = !LEDA1;
          printf("#$#$#$#$#$#$#$#Reiceive message from CANRx_ID: %#x #$#$#$#$#$#$#$# \r\n", CANRx_ID);
          //printf("**received data 1** \r\n");
          printf("**receive_(data0,data1,data2,data3,data4,data5) = (%#x, %#x, %#x, %#x, %#x, %#x )** \r\n",Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5);
          printf("++++++++message finish sending++++++++++ \r\n");
          //	 printf(" \r\n");
          CanRxFlag = DISABLE;
        }


    }

}

void Node2()
{
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
//          for(int mestimes = 0; mestimes < MesTimesInBC; mestimes ++)
//            {
//              printf("finalTimerValue[%d] is %d \r\n", mestimes, finalTimerValue[mestimes]);
//            }

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
          //     printf("##########interrupt %d ########### \r\n",interrupt_sos_times);
          temp_sos_ID = SOS_ID();
          //     printf("##########end interrupt########### \r\n");
          //    printf(" \r\n");
          temp_MesTranTime = MesTransmitTime(temp_sos_ID);
          // printf("Exact MesTranTime=: %d \r\n", temp_MesTranTime);
          interrupt_sos_times ++ ;
          SOS_ISR_flag = DISABLE;
        }

      if(CanRxFlag == ENABLE )
        {
          LEDA1 = !LEDA1;
          printf("**received data** \r\n");
          printf("#$#$#$#$#$#$#$#Reiceive message from CANRx_ID: %#x #$#$#$#$#$#$#$# \r\n", CANRx_ID);
          printf("**receive_(data0,data1,data2,data3,data4,data5) = (%#x, %#x, %#x, %#x, %#x, %#x )** \r\n",Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5);
          printf("++++++++message finish sending++++++++++ \r\n");
          printf(" \r\n");
          CanRxFlag = DISABLE;
        }


    }
}



