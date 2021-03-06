
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "systick.h" 
#include <stdlib.h>
#include <stdio.h>


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/
uint16_t CAN_ID;
uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
uint8_t CanFlag,Display;

/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void USART_Configuration(void);
void NVIC_Configuration(void);
void CAN_Configuration(void);
void CanWriteData(uint16_t ID);


/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
int main(void)
{
	delay_init();
  CAN_Configuration();
	USART_Configuration();
	printf("\r\n");
	printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
	printf("*  CAN TEST??^_^  *\r\n");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");
  printf("CAN-Bus Speed 100kHz \r\n");
    /* Infinite loop */
    while (1)
	{ 
	  if( CanFlag == ENABLE )
	  {
	  CanFlag = DISABLE;
		printf("CAN Receive Data \r\n");
		printf("CAN ID %x \r\n",CAN_ID);
		printf("CAN_DATA0 %x \r\n",CAN_DATA0);
		printf("CAN_DATA1 %x \r\n",CAN_DATA1);
		printf("CAN_DATA2 %x \r\n",CAN_DATA2);
		printf("CAN_DATA3 %x \r\n",CAN_DATA3);
		printf("CAN_DATA4 %x \r\n",CAN_DATA4);
		printf("CAN_DATA5 %x \r\n",CAN_DATA5);
		printf("CAN_DATA6 %x \r\n",CAN_DATA6);
		printf("CAN_DATA7 %x \r\n",CAN_DATA7);
	  }
	  
	  CanWriteData(0xA5A5);

      if( Display ) 
	  { 
        /*====LED-ON=======*/
	    GPIO_SetBits(GPIOC , GPIO_Pin_13);
			GPIO_SetBits(GPIOC , GPIO_Pin_14);
			printf("lighting up led c13\r\n");
//	    GPIO_SetBits(GPIOC , GPIO_Pin_7);
//	    GPIO_SetBits(GPIOF , GPIO_Pin_8);
//	    GPIO_SetBits(GPIOF , GPIO_Pin_9);	 
	  }  
	  else 
	  { 
	    /*====LED-OFF=======*/ 
	    GPIO_ResetBits(GPIOC , GPIO_Pin_13);
			GPIO_ResetBits(GPIOC , GPIO_Pin_14);
			printf("turing off led c13\r\n");
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_7);
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_8);
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_9);		 
	  }
	  Display = ~Display;   	
	  delay_ms(200);  /* delay 200ms */  
    }
}

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
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // enable can1 clock.
  //GPIO_PinRemapConfig(GPIO_Remap1_CAN1,ENABLE); 
  
	/* Configure CAN pin: RX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);   
  
	/* Configure CAN pin: TX */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);	     
				 
	
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13 |GPIO_Pin_14;// | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	
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
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn; // mail x0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure USART1 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

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

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART1, &USART_InitStructure); 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  USART_Cmd(USART1, ENABLE);
}

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

  NVIC_Configuration();
  GPIO_Configuration(); // configure the can IO
	
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = DISABLE; /* ʱ?䴥????ֹ, ʱ?䴥????CANӲ?????ڲ???ʱ??????????ұ????ڲ???ʱ???? */
  CAN_InitStructure.CAN_ABOM = DISABLE; /* ?Զ????߽?ֹ???Զ????ߣ?һ??Ӳ?????ص?128??11??????λ?????Զ??˳?????״̬????????Ҫ?????趨???????˳? */
  CAN_InitStructure.CAN_AWUM = ENABLE;  /* ?Զ????ѽ?ֹ???б???????ʱ???Զ??˳?????	*/
  CAN_InitStructure.CAN_NART = DISABLE; /* ?????ش?, ????????һֱ?????ɹ?ֹ??????ֻ??һ?? */
  CAN_InitStructure.CAN_RFLM = DISABLE; /* receive FIFO lock, 1-- after locking, newly coming data will be ignored.0--???յ??µı????򸲸?ǰһ????	*/
  CAN_InitStructure.CAN_TXFP = ENABLE;  /* FIFO priority, 0 means decide by identifier, 1 means decided by request sequence	*/
	CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; /* ģʽ	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* ģʽ	*/
  CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;      /* ????ͬ????????ֻ??canӲ?????ڳ?ʼ??ģʽʱ???ܷ????????Ĵ??? */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;      /* ʱ????1 */
  CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;      /* ʱ????2 */
  CAN_InitStructure.CAN_Prescaler = 45;         /* ??????Ԥ??Ƶ?? */  
 
  /* ?????ʼ??㷽?? */
  /* CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  ?˴?????Ϊ  CANbps=36000000/(45*(4+3+1))=100kHz */   														  //?˴?Tseg1+1 = CAN_BS1_8tp
  /* ???ô?????: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW */
  // wang configure 
	CAN_Init(CAN1,&CAN_InitStructure);
  if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED) 		
  {
    /* ??ʼ??ʱ??????CAN_MCR?ĳ?ʼ??λ */  														
    /* Ȼ???鿴Ӳ???Ƿ???????????CAN_MSR?ĳ?ʼ??λ??ȷ???Ƿ??????˳?ʼ??ģʽ  */		 													                  
  }	
  /* ????CAN?????? */
  /* 32λ??Ӧ??id */
  /* stdid[10:0]??extid[17:0],ide,rtr	*/
  /* 16λ??Ӧ??id */
  /* stdid[10:0],ide,rtr,extid[17:15] */
  /* һ??ʹ??????ģʽ	*/
  /* Ҫע??????fifo???մ??????жϣ????о???fifo?ĸ????ȡ??һֱ????????һ?????ݣ? Ҫ?ͷŲ???ȡ??һ?????? */
  /* ??ʹ?õ??ж??? */
  /* 1,????Ϣ?жϣ???fifo?Һ??ж? */
  /* 2,fifo???ж?	*/
  /* 3,fifo??֮????????Ϣ?????жϣ???fifo?????ж?	*/
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;  /* ?ܹ?ͨ???ù??????ı??Ĵ浽fifo0?? */
  CAN_FilterInitStructure.CAN_FilterNumber=0;     /* ??????0 */
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; /* 32λ */
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;  /*which part needs to be filtered should check can filter part, also yehuo fourth part 18.19min shows in details*/
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000; //according to what you want, the filte is the location that you want to keep the same then can be kept
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;  
  
 
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* ?Һ??ж?, ?????жϺ???fifo?ı??ĺ????ͷű??????жϱ?־ */
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
void CanWriteData(uint16_t ID)
{
  CanTxMsg TxMessage;
  
	// create random values here
  CAN_DATA0=rand()%0xff;  CAN_DATA1=rand()%0xff;  
  CAN_DATA2=rand()%0xff;  CAN_DATA3=rand()%0xff;  
  CAN_DATA4=rand()%0xff;  CAN_DATA5=rand()%0xff;
  CAN_DATA6=rand()%0xff;  CAN_DATA7=rand()%0xff; 

  /* transmit */
  TxMessage.StdId = ID;  /* ???ñ?׼id  ע????׼id??????7λ????ȫ??????(1)????11λ */
//TxMessage.ExtId = 0x00;     //??????չid  ??չid??18λ
  TxMessage.RTR = CAN_RTR_DATA; /* data frame*/
  TxMessage.IDE = CAN_ID_STD;   /* ʹ?ñ?׼id	*/
  TxMessage.DLC = 8;            /* ???ݳ???, can???Ĺ涨?????????ݳ???Ϊ8?ֽ? */
  TxMessage.Data[0] = CAN_DATA0;    
  TxMessage.Data[1] = CAN_DATA1;    
  TxMessage.Data[2] = CAN_DATA2;    
  TxMessage.Data[3] = CAN_DATA3;    
  TxMessage.Data[4] = CAN_DATA4;    
  TxMessage.Data[5] = CAN_DATA5;     
  TxMessage.Data[6] = CAN_DATA6;    
  TxMessage.Data[7] = CAN_DATA7;      
  CAN_Transmit(CAN1,&TxMessage);  /* ??????????Ϣ???????͵???????0,1,2??û?????????뷢??no_box */	
}

/*******************************************************************************
* Function Name  : USB_LP_CAN1_RX0_IRQHandler
* Description    : This function handles USB Low Priority or CAN RX0 interrupts 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
//void USB_LP_CAN1_RX0_IRQHandler(void) 
//{
//  CanRxMsg RxMessage;
//  CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);  /* ?˺????????ͷ??????????˵?,?ڷǱ?Ҫʱ,????Ҫ?Լ??ͷ? */
//  CAN_ID=RxMessage.StdId;
//  CAN_DATA0=RxMessage.Data[0];
//  CAN_DATA1=RxMessage.Data[1];
//  CAN_DATA2=RxMessage.Data[2];
//  CAN_DATA3=RxMessage.Data[3];
//  CAN_DATA4=RxMessage.Data[4];
//  CAN_DATA5=RxMessage.Data[5];
//  CAN_DATA6=RxMessage.Data[6];
//  CAN_DATA7=RxMessage.Data[7];
//  CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);  /* ?????????ж? */
//  CanFlag = ENABLE;
//}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

