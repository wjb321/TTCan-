#include "usart.h"

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : configure CAN and LED IO
* Input          : None
* Output         : None
* Return         : None
* Attention		   :  1) if the AFIO function is needed, then should use PinRemapConfig function, the input para can be 
*                  Remap_1 or Remap_2.
                    2) the clock for gpio is bus2 and for can is bus1.
                    3) as for the GPIO mode, should check reference manual in 9.1.11, table 28 , the rx can be floating or pull-up
*******************************************************************************/

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* CAN Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC ,ENABLE); // pin reuse configure| RCC_APB2Periph_AFIO

  //GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE); 
  
	/* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
  
	/* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	     
				 
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 |GPIO_Pin_14;// | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}


void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 
  NVIC_InitTypeDef NVIC_InitStructure;
 
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1,ENABLE);  
  /*
  *  USART1_TX -> PA9 , USART1_RX ->	PA10
  */				
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);		   

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	        
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器


  USART_InitStructure.USART_BaudRate = 921600;//256000;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures the nested vectored interrupt controller.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : here are two different priorities
                 preemptionpriority and response priority, first compare the preemption priority then 
                 compare the respinse priority, the number is smaller, the priority is higher
                 normally the interrupt priority is set only once in the beginnining.
*******************************************************************************/
void NVIC0_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; // mail x0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void NVIC1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure1;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  NVIC_InitStructure1.NVIC_IRQChannel = CAN1_RX1_IRQn ;   //// mail x1
  NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure1);
}

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	// USART_RX_STA if bit15 is 1 then it is ox8000((2^3), if bit14 is 1 then it is 0x4000(2^2)
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
} 
#endif	

