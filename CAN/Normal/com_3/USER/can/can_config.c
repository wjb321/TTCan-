#include "can_config.h"
#include "usart.h"

uint32_t mask_id = 0x1314;
uint32_t CAN_ID;
uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
uint8_t CanFlag; //,Display


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
  CAN_InitTypeDef        CAN_InitStructure;  // CAN mode struct
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;  // CAN filter struct
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // enable can1 clock.
	
  NVIC_Configuration();
  GPIO_Configuration(); // configure the can IO
	
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
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;      /* 重新同步跳宽，只有can硬件处于初始化模式时才能访问这个寄存器 */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_5tq;      /* 时间段1 */
  CAN_InitStructure.CAN_BS2 = CAN_BS2_6tq;      /* 时间段2 */
  CAN_InitStructure.CAN_Prescaler = 7;         /* 波特率预分频数 */  
 
  /* 波特率计算方法 */
  /* CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  此处计算为  CANbps=36000000/(45*(4+3+1))=100kHz */   														  //此处Tseg1+1 = CAN_BS1_8tp
  /* 配置大方向: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW */
  // wang configure 
	CAN_Init(CAN1,&CAN_InitStructure);
  if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
  {
    /* 初始化时先设置CAN_MCR的初始化位 */  														
    /* 然后查看硬件是否真的设置了CAN_MSR的初始化位来确认是否进入了初始化模式  */		 													                  
  }	
  /* 配置CAN过滤器 */
  /* 32位对应的id */
  /* stdid[10:0]，extid[17:0],ide,rtr	*/
  /* 16位对应的id */
  /* stdid[10:0],ide,rtr,extid[17:15] */
  /* 一般使用屏蔽模式	*/
  /* 要注意的是fifo接收存满了中断，还有就是fifo的概念，即取的一直是最早那一个数据， 要释放才能取下一个数据 */
  /* 常使用的中断有 */
  /* 1,有信息中断，即fifo挂号中断 */
  /* 2,fifo满中断	*/
  /* 3,fifo满之后又有信息来则中断，即fifo溢出中断	*/
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;  /* 能够通过该过滤器的报文存到fifo0中 */
  CAN_FilterInitStructure.CAN_FilterNumber=0;     /* 过滤器0 */
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
	/*
#define CAN_FilterMode_IdMask       ((uint8_t)0x00) 
#define CAN_FilterMode_IdList       ((uint8_t)0x01) 
	here are two different modes
*/
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; /* 32位 */
	 CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;  /*which part needs to be filtered should check can filter part, also yehuo fourth part 18.19min shows in details*/
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000; //according to what you want, the filte is the location that you want to keep the same then can be kept
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;  
  
//	CAN_FilterInitStructure.CAN_FilterIdHigh= ((((u32)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//要筛选的ID高位 
//	CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)0x1314<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //要筛选的ID低位 
//	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<3 |CAN_ID_STD|CAN_RTR_DATA )&0xffff0000)>>16;
////	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<21)&0xffff0000);
//  CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)mask_id<<3)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
//  CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0xFFFF;   //????
//  CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0xFFFF;   //????
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* 挂号中断, 进入中断后读fifo的报文函数释放报文清中断标志 */
  CAN_FilterInit(&CAN_FilterInitStructure);
}

/*******************************************************************************
* Function Name  : CanWriteData
* Description    : Can Write Date to CAN-BUS
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void CanWriteData(uint32_t ID)
{
	uint8_t box;
  CanTxMsg TxMessage;
//  CAN_DATA0=01;  CAN_DATA1=02;  
//  CAN_DATA2=03;  CAN_DATA3=04;  
//  CAN_DATA4=05;  CAN_DATA5=07;
//  CAN_DATA6=010;  CAN_DATA7=011; 
	// create random values here
  CAN_DATA0=rand()%0xff;  CAN_DATA1=rand()%0xff;  
  CAN_DATA2=rand()%0xff;  CAN_DATA3=rand()%0xff;  
  CAN_DATA4=rand()%0xff;  CAN_DATA5=rand()%0xff;
  CAN_DATA6=rand()%0xff;  CAN_DATA7=7; 

  /* transmit */
  TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
//TxMessage.ExtId = 0x00;     //设置扩展id  扩展id共18位
  TxMessage.RTR = CAN_RTR_DATA; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_STD;   /* 使用标准id	*/
  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
//	
//	  //TxMessage.StdId = ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
//TxMessage.ExtId = ID;     //设置扩展id  扩展id共18位
//  TxMessage.RTR = CAN_RTR_DATA; /* data frame*/
//  TxMessage.IDE = CAN_ID_EXT  ;//CAN_ID_STD;   /* 使用标准id	*/
//  TxMessage.DLC = 8;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	
	
//   TxMessage.Data[0] = 10;  	
  TxMessage.Data[0] = CAN_DATA0;    
  TxMessage.Data[1] = CAN_DATA1;    
  TxMessage.Data[2] = CAN_DATA2;    
  TxMessage.Data[3] = CAN_DATA3;    
  TxMessage.Data[4] = CAN_DATA4;    
  TxMessage.Data[5] = CAN_DATA5;     
  TxMessage.Data[6] = CAN_DATA6;    
  TxMessage.Data[7] = CAN_DATA7;      
  box = CAN_Transmit(CAN1,&TxMessage);  /* 返回这个信息请求发送的邮箱号0,1,2或没有邮箱申请发送no_box */	
	while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	printf("\r\n the data has been sent already \r\n");
	
}
