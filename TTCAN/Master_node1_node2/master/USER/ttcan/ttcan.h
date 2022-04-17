#ifndef TTCAN_H
#define TTCAN_H

#include "stm32f10x.h"
#include <stdio.h>
#include "TTCAN_mes.h"

#define TotNumBC 4
#define NumSlot 5
#define SlotTime 1
#define MaskID_SM  0x001
#define MaskID_Ref 0x002

#define mes1_ID 0x120
#define mes2_ID 0x121
#define mes3_ID 0x122
#define mes4_ID 0x123
#define mes5_ID 0x124
#define mes6_ID 0x125
void node(void);
void SendMatrix(uint16_t ID);
void MatrixReiceive();

#endif
