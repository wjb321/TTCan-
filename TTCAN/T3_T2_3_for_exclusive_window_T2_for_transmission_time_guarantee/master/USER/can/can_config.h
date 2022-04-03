#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H
#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 
#include "ttcan.h"

void CAN_Configuration(void);
void SendRef(uint16_t ID, uint16_t TOTNumBC, uint16_t NUMSlot, uint16_t SlOTTime);//uint16_t NumBC

#endif 
