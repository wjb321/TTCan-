#ifndef NODES_H
#define NODES_H
#include "ttcan.h"
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 

extern uint16_t  Received_mes_id[TotNumBC][NumSlot];
extern uint8_t received_is_exclusive_window[TotNumBC][NumSlot];
extern uint16_t SM_received_counter; // from isr
extern uint16_t SOS_ISR_flag;
extern uint16_t TimerArray[NumSlot-2];
extern uint16_t finalTimerValue[NumSlot];
extern int finalTimerArrayShift_Nonzero;
extern int finalTimerArrayShift_zero; // 标志最终时间数组中没有从0 开始的情况[0, 300, XXX,...]
extern int TimerArrayLocation;
extern uint8_t CanRxFlag;
extern uint16_t CANRx_ID;
extern uint8_t Transmit_time_flag;
extern uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
extern uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5;
extern uint16_t timeStamp,TimeStampRx1;
extern uint16_t Slot_ISR_Timer, mes_id[TotNumBC][NumSlot];
extern uint16_t CanRefFlag, CanSMFlag;
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

void Node0(void);
void Node1(void);
void Node2(void);
#endif 