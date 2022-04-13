#include "TTCAN_mes.h"
#include "can_config.h"

uint8_t MES1_DATA0,MES1_DATA1,MES1_DATA2,MES1_DATA3,MES1_DATA4,MES1_DATA5,MES1_DATA6,MES1_DATA7;
uint8_t MES2_DATA0,MES2_DATA1,MES2_DATA2,MES2_DATA3,MES2_DATA4,MES2_DATA5,MES2_DATA6,MES2_DATA7;
uint8_t MES3_DATA0,MES3_DATA1,MES3_DATA2,MES3_DATA3,MES3_DATA4,MES3_DATA5,MES3_DATA6,MES3_DATA7;
uint8_t MES4_DATA0,MES4_DATA1,MES4_DATA2,MES4_DATA3,MES4_DATA4,MES4_DATA5,MES4_DATA6,MES4_DATA7;
uint8_t MES5_DATA0,MES5_DATA1,MES5_DATA2,MES5_DATA3,MES5_DATA4,MES5_DATA5,MES5_DATA6,MES5_DATA7;
int64_t Mes1_TimeTx =0; int64_t Mes2_TimeTx =0; int64_t Mes3_TimeTx =0; int64_t Mes4_TimeTx =0; int64_t Mes5_TimeTx =0; 
void message1(uint16_t ID1)
{
	uint8_t box;
  CanTxMsg TxMessage;

	MES1_DATA0=0x1;  MES1_DATA1=0x2;  
  MES1_DATA2=0x3;  MES1_DATA3=0x4;  
  MES1_DATA4=0x5;  MES1_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID1;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = MES1_DATA0;    
  TxMessage.Data[1] = MES1_DATA1;    
  TxMessage.Data[2] = MES1_DATA2;    
  TxMessage.Data[3] = MES1_DATA3;    
  TxMessage.Data[4] = MES1_DATA4;    
  TxMessage.Data[5] = MES1_DATA5;  
	
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	
	/* transmit message wang adds */
	Mes1_TimeTx += TxMessage.TimeStampTx;
	TxMessage.TimeStampTx =0;
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("timeTx stamp %lld \r\n", Mes1_TimeTx);
	//printf("\r\n the data has been sent already \r\n");
}

void message2(uint16_t ID2)
{
	uint8_t box;
  CanTxMsg TxMessage;

	MES1_DATA0=0x1;  MES1_DATA1=0x2;  
  MES1_DATA2=0x3;  MES1_DATA3=0x4;  
  MES1_DATA4=0x5;  MES1_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID2;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = MES2_DATA0;    
  TxMessage.Data[1] = MES2_DATA1;    
  TxMessage.Data[2] = MES2_DATA2;    
  TxMessage.Data[3] = MES2_DATA3;    
  TxMessage.Data[4] = MES2_DATA4;    
  TxMessage.Data[5] = MES2_DATA5;  
	
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	
	/* transmit message wang adds */
	Mes2_TimeTx += TxMessage.TimeStampTx;
	TxMessage.TimeStampTx =0;
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("timeTx stamp %lld \r\n", Mes2_TimeTx);
	//printf("\r\n the data has been sent already \r\n");
}

void message3(uint16_t ID3)
{
		uint8_t box;
  CanTxMsg TxMessage;

	MES1_DATA0=0x1;  MES1_DATA1=0x2;  
  MES1_DATA2=0x3;  MES1_DATA3=0x4;  
  MES1_DATA4=0x5;  MES1_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID3;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = MES2_DATA0;    
  TxMessage.Data[1] = MES2_DATA1;    
  TxMessage.Data[2] = MES2_DATA2;    
  TxMessage.Data[3] = MES2_DATA3;    
  TxMessage.Data[4] = MES2_DATA4;    
  TxMessage.Data[5] = MES2_DATA5;  
	
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	
	/* transmit message wang adds */
	Mes3_TimeTx += TxMessage.TimeStampTx;
	TxMessage.TimeStampTx =0;
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("timeTx stamp %lld \r\n", Mes3_TimeTx);
	//printf("\r\n the data has been sent already \r\n");
}


void message4(uint16_t ID4)
{
		uint8_t box;
  CanTxMsg TxMessage;

	MES1_DATA0=0x1;  MES1_DATA1=0x2;  
  MES1_DATA2=0x3;  MES1_DATA3=0x4;  
  MES1_DATA4=0x5;  MES1_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID4;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = MES4_DATA0;    
  TxMessage.Data[1] = MES4_DATA1;    
  TxMessage.Data[2] = MES4_DATA2;    
  TxMessage.Data[3] = MES4_DATA3;    
  TxMessage.Data[4] = MES4_DATA4;    
  TxMessage.Data[5] = MES4_DATA5;  
	
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	
	/* transmit message wang adds */
	Mes4_TimeTx += TxMessage.TimeStampTx;
	TxMessage.TimeStampTx =0;
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("timeTx stamp %lld \r\n", Mes4_TimeTx);
	//printf("\r\n the data has been sent already \r\n");
}


void message5(uint16_t ID5)
{
	uint8_t box;
  CanTxMsg TxMessage;

	MES1_DATA0=0x1;  MES1_DATA1=0x2;  
  MES1_DATA2=0x3;  MES1_DATA3=0x4;  
  MES1_DATA4=0x5;  MES1_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID5;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = MES5_DATA0;    
  TxMessage.Data[1] = MES5_DATA1;    
  TxMessage.Data[2] = MES5_DATA2;    
  TxMessage.Data[3] = MES5_DATA3;    
  TxMessage.Data[4] = MES5_DATA4;    
  TxMessage.Data[5] = MES5_DATA5;  
	
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	
	/* transmit message wang adds */
	Mes5_TimeTx += TxMessage.TimeStampTx;
	TxMessage.TimeStampTx =0;
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("timeTx stamp %lld \r\n", Mes5_TimeTx);
	//printf("\r\n the data has been sent already \r\n");
}