#include "can_config.h"
#include "usart.h"
#include "ttcan.h"

uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
void CanInit();
void CanFilter_0_Configure();
void 	CanFilter_1_Configure();
/*******************************************************************************
* Function Name  : CAN_mode_Configuration
* Description    : contains CAN mode and filter congiguration
* Input          : None
* Output         : None
* Return         : None
* Attention		   : 1) for testing, better use loopback mode, after testing then use normal mode
*******************************************************************************/
void CAN_Configuration(void)
{ 
	CanInit();
	CanFilter_0_Configure();
	CanFilter_1_Configure();
  NVIC0_Configuration();
  NVIC1_Configuration();	
  GPIO_Configuration(); // configure the can IO
} 


void CanInit()
{
	CAN_InitTypeDef        CAN_InitStructure;  // CAN mode struct
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // enable can1 clock.
	/* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = ENABLE; /* 时间触发禁止, 时间触发：CAN硬件的内部定时器被激活，并且被用于产生时间戳 */
  CAN_InitStructure.CAN_ABOM = ENABLE; /* 自动离线禁止，自动离线：一旦硬件监控到128次11个隐性位，就自动退出离线状态。在这里要软件设定后才能退出 */
  CAN_InitStructure.CAN_AWUM = ENABLE;  /* 自动唤醒禁止，有报文来的时候自动退出休眠	*/
  CAN_InitStructure.CAN_NART = DISABLE; /* 报文重传, 如果错误一直传到成功止，否则只传一次 */
  CAN_InitStructure.CAN_RFLM = DISABLE; /* receive FIFO lock, 1-- after locking, newly coming data will be ignored.0--接收到新的报文则覆盖前一报文	*/
  CAN_InitStructure.CAN_TXFP = DISABLE;  /* FIFO priority, 0 means decide by identifier, 1 means decided by request sequence	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; /*better for testing. when 2 boards communication, then it is mode_normal 	*/
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* 模式	*/
	//CAN_InitStructure.CAN_Mode = CAN_Mode_Silent ;
  CAN_InitStructure.CAN_SJW = CAN_SJW_4tq;      /* 重新同步跳宽，只有can硬件处于初始化模式时才能访问这个寄存器 */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;      /* PTS + PBS1 */
  CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;      /* PBS2 */
  CAN_InitStructure.CAN_Prescaler = 45;         /* 波特率预分频数 */  
	CAN_Init(CAN1,&CAN_InitStructure);
  if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
  {	 													                  
  }	
}

void CanFilter_0_Configure()
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure0;  // CAN filter struct
	CAN_FilterInitStructure0.CAN_FilterActivation=ENABLE;
	CAN_FilterInitStructure0.CAN_FilterFIFOAssignment=0;  /* 能够通过该过滤器的报文存到fifo0中 */
  CAN_FilterInitStructure0.CAN_FilterNumber=0;     /* 过滤器0 */
  CAN_FilterInitStructure0.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure0.CAN_FilterScale=CAN_FilterScale_32bit; /* 32位 */
	CAN_FilterInitStructure0.CAN_FilterIdHigh= ((((u32)MaskID_SM<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF0000)>>16;	
	CAN_FilterInitStructure0.CAN_FilterIdLow= (((u32)MaskID_SM<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF; 
	CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* 挂号中断, 进入中断后读fifo的报文函数释放报文清中断标志 */
	CAN_FilterInitStructure0.CAN_FilterMaskIdHigh  = 0xFFFF;   
  CAN_FilterInitStructure0.CAN_FilterMaskIdLow   = 0xFFFF; 
  CAN_FilterInit(&CAN_FilterInitStructure0);
}
void CanFilter_1_Configure()
{
	CAN_FilterInitTypeDef  CAN_FilterInitStructure1;  // CAN filter struct
	CAN_FilterInitStructure1.CAN_FilterActivation=ENABLE;
	CAN_FilterInitStructure1.CAN_FilterFIFOAssignment=1;  /* 能够通过该过滤器的报文存到fifo0中 */
  CAN_FilterInitStructure1.CAN_FilterNumber=1;     /* 过滤器0 */
  CAN_FilterInitStructure1.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure1.CAN_FilterScale=CAN_FilterScale_32bit; /* 32位 */
	CAN_FilterInitStructure1.CAN_FilterIdHigh= ((((u32)MaskID_Ref<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF0000)>>16;	
	CAN_FilterInitStructure1.CAN_FilterIdLow= (((u32)MaskID_Ref<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF; 
  CAN_ITConfig(CAN1,CAN_IT_FMP1, ENABLE);   /* 挂号中断, 进入中断后读fifo的报文函数释放报文清中断标志 */
	CAN_FilterInitStructure1.CAN_FilterMaskIdHigh  = 0xFFFF;   
  CAN_FilterInitStructure1.CAN_FilterMaskIdLow   = 0xFFFF; 
  CAN_FilterInit(&CAN_FilterInitStructure1);
}


void NodeMesTransmit(uint16_t ID)
{ 
	
	uint8_t box;
  CanTxMsg TxMessage;
	
	CAN_DATA0=0x22;  CAN_DATA1=0x22;  
  CAN_DATA2=0x22;  CAN_DATA3=0x22;  
  CAN_DATA4=0x5;  CAN_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e; 

  /* transmit */
  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  TxMessage.ExtId = ID;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_EXT   ;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
  TxMessage.Data[0] = CAN_DATA0;    
  TxMessage.Data[1] = CAN_DATA1;    
  TxMessage.Data[2] = CAN_DATA2;    
  TxMessage.Data[3] = CAN_DATA3;    
  TxMessage.Data[4] = CAN_DATA4;    
  TxMessage.Data[5] = CAN_DATA5;  
//  TxMessage.Data[6] = CAN_DATA6;    
//  TxMessage.Data[7] = CAN_DATA7;      
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	 delay_ms(100);
	/* transmit message wang adds */
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);	
	printf(">>>>>ID  %#x is sent<<<<<\r\n", ID);
}
