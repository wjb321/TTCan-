#ifndef TTCAN_MES_H
#define TTCAN_MES_H

#include "stm32f10x.h"
#include "ttcan.h"


#define MES1 1
#define MES2 2
#define MES3 3
#define MES4 4
#define MES5 5


void message1(uint16_t ID1);
void message2(uint16_t ID2);
void message3(uint16_t ID3);
void message4(uint16_t ID4);
void message5(uint16_t ID5);

#endif  