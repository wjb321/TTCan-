#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 


void CanInit(void);
//void CanFilter_01_Configure(uint16_t FifoNum, uint16_t FilterNum, uint32_t maskidSM ,uint32_t maskidRef);
void CAN_Configuration(void);
void CanWriteData(uint16_t ID);
void CanFilter_0_Configure(void);
void CanFilter_1_Configure(void);
void NodeMesTransmit(uint16_t ID);
void SendRef(uint16_t ID, uint16_t TOTNumBC, uint16_t NUMSlot, uint16_t SLOTTime);
#endif 
