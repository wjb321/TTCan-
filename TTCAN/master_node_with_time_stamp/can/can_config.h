#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 



void CAN_Configuration(void);
void CanWriteData(uint16_t ID, uint16_t BCTime, uint16_t NumBC, uint16_t NumSlot);

#endif 
