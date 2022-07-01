#ifndef CAN_CONFIG_H
#define CAN_CONFIG_H

#include "stm32f10x.h"
#include <stdio.h>
#include "systick.h" 

/*slave node*/
void CanInit(void);
void CAN_Configuration(void);
void CanWriteData(uint16_t ID);
void CanFilter_0_Configure(void);
void CanFilter_1_Configure(void);
void NodeMesTransmit(uint16_t ID);

/*master node*/
void CAN__Master_Configuration(void);
void SendRef(uint16_t ID, uint16_t TOTNumBC, uint16_t NUMSlot, uint16_t SLOTTime);
#endif 
