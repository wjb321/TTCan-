#ifndef TTCAN_H
#define TTCAN_H

#include "stm32f10x.h"
#include <stdio.h>
#define TotNumBC 4
#define NumSlot 5
#define SlotTime 1
#define NumMes 6
#define MesList 3
#define MaskID_SM  0x001
#define MaskID_Ref 0x002


#define mes1_ID 0x120
#define mes2_ID 0x121
#define mes3_ID 0x122

// uint16_t Node1MesIDList[3] = {mes1_ID, mes2_ID, mes3_ID};//, mes4_ID, mes5_ID, mes6_ID};

#define mes4_ID 0x123
#define mes5_ID 0x124
#define mes6_ID 0x125
void node(void);
void SendMatrix(uint16_t ID);
void ReiceiveSM(void);
void SlotTimer(void);
void TimerSlotSet(void);
uint16_t SOS_ID(void);
void MesTransmit(void);
int MesTransmitTime(uint16_t ID);
//void SlotTimeArray();
int TimerISR(void);
uint16_t ArraySum(uint16_t * inputarray, uint16_t location);
void meslocation(void);
void FinalLocation(void);
typedef int (* tempMesLocationInSM)[2];
int TimesOfBCMes(tempMesLocationInSM tempMesLoca);

tempMesLocationInSM GetLocationInRxSM(int b[][2]);

#endif
