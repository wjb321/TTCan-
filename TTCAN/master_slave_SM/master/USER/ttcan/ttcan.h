#ifndef TTCAN_H
#define TTCAN_H

#include "stm32f10x.h"
#include <stdio.h>
#include "TTCAN_mes.h"

#define TotNumBC 4
#define NumSlot 5
#define SlotTime 1
#define MaskID_SM  0x0001
#define MaskID_Ref 0x0002

#define mes1_ID 0x2620 
#define mes2_ID 0x2621 
#define mes3_ID 0x2622 
#define mes4_ID 0x2623
#define mes5_ID 0x2624 
void node(void);
void SendMatrix(uint16_t ID);
void MatrixReiceive();

#endif  
