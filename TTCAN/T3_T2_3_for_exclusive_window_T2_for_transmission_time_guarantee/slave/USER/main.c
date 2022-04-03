#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

	#include "stm32f10x.h"
#include "systick.h" 
#include "usart.h" 
#include <stdlib.h>
#include <stdio.h>
#include "led.h"
#include "can_config.h"
#include "timer.h"
#include "ttcan.h"
	
#define   MasterNode 0
#define   SlaveNode1 1
#define   SlaveNode2 2


/* Private variables ---------------------------------------------------------*/
extern uint8_t Transmit_time_flag;
extern uint8_t Rx_DATA0,Rx_DATA1,Rx_DATA2,Rx_DATA3,Rx_DATA4,Rx_DATA5,Rx_DATA6,Rx_DATA7;
extern uint16_t Slot_ISR_Timer, mes_id[TotNumBC][NumSlot];
extern uint8_t CanRefFlag, CanSMFlag; 
extern uint16_t NumBC, Mes_list[NumMes][MesList];
extern uint16_t CANRef_ID, CANSM_ID;	
static void can_delay(__IO u32 nCount);
extern int i , j;
int sm_id_nonzero_flag = 0;
extern uint16_t  Received_mes_id[TotNumBC][NumSlot];
extern uint8_t received_is_exclusive_window[TotNumBC][NumSlot];
extern uint16_t SM_received_counter; // from isr
extern 	uint16_t SOS_ISR_flag;
extern	uint8_t SOS_ISR_Count;
extern	uint16_t TimerArray[NumSlot-2];
int TimerArray_p = 0;
//void mes_ids();
void Node0(void);
void Node1(void);
void Node2(void);


int main(void)
{ 
	delay_init();
	LED_Init();
	USART_Configuration();
  CAN_Configuration();
  
	printf("\r\n");
	printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
	printf("*  node 1 gets the reference message");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");
	TIM3_Int_Init(3500,7199,0);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s 
	TIM2_Int_Init(3500,7199,0);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s 
	
	switch(SlaveNode2) //SlaveNode2
	{
		case 0:Node0(); break;
		case 1:Node1(); break;
		case 2:Node2(); break;
		default: break;
	}

}


void Node0()
{
	
}

void Node1()
{
	
}

void Node2()
{
  uint16_t temp_receive_id, first_mes_after_ref= 0;
	uint16_t temp_MesTranTime = 0, temp_sos_ID, interrupt_sos_times = 0, temp= 10000;
  
 	while (1)
	{  	 
    /*Reiceive SM from master node*/
		if( CanSMFlag == ENABLE )
	  {
		  LEDC13 = !LEDC13;	 
	    CanSMFlag = DISABLE;
      ReiceiveSM();
			printf("SM ID is sent: %#x \r\n", CANRef_ID);
	    NumBC = 0;   	
     }
		
		/*Reiceive reference and start BC*/
		
		 if( CanRefFlag == ENABLE )
	   {  
			 
		
				LEDC14 = !LEDC14;	
				CanRefFlag = DISABLE;
				TimerArray_p = 0;		
			 TimerSlotSet();	// print timer array is:...
			 temp_receive_id= Received_mes_id[0][1];//NumBC
			 first_mes_after_ref = MesTransmitTime(temp_receive_id);
				//T3 for counting the time slots
				TIM3_Int_Init(TimerArray[0],7199,ENABLE); // this gets from timerset()
			printf("tim3 %d \r\n", TimerArray[0]);	
			//T2 for counting the mes transmission time(some message can not occupy fully respectice allocated time slot)
			//then should check its PSA list to get exact transmission time.
			TIM2_Int_Init(first_mes_after_ref,7199,ENABLE);
			printf("initial ID %#x\r\n",temp_receive_id );	
			printf("tim2ref init time %d \r\n",first_mes_after_ref );	
			printf("TimerArray[0]=: %d \r\n", TimerArray[0]);	
      ++NumBC;			
			if(NumBC > TotNumBC){
	    NumBC = 0;}
			printf("Reference ID is sent: %#x \r\n", CANSM_ID);	
  	  SOS_ISR_Count = 0 ;
      interrupt_sos_times =0;				
     printf("************reference ********** \r\n");
		 printf("            reference           \r\n" );
     printf("************reference********** \r\n");			
	    }

		 /*SOS triggered internal messages*/
		 if(SOS_ISR_flag == ENABLE )
		 { 
		
			LEDA0 = !LEDA0;
			temp_sos_ID = SOS_ID();	
      interrupt_sos_times ++;				 
			printf("##########interrupt %d ########### \r\n" ,interrupt_sos_times);
      printf("SOS_ID()=: %#x \r\n", temp_sos_ID);	
			printf("##########interrupt########### \r\n");	
			temp_MesTranTime = MesTransmitTime(temp_sos_ID);
			 printf("MesTranTime=: %d \r\n", temp_MesTranTime);	
		  TIM2_Int_Init(temp_MesTranTime ,7199,ENABLE);
			SOS_ISR_flag = DISABLE;
		 }
     
		 		 /*SOS triggered internal messages*/
//      if(Transmit_time_flag)
//			{
//				
//			}
    
		 		 
		   if(Transmit_time_flag == ENABLE )
			{    
				   printf("++++++++message start sending++++++++++ \r\n");	
	         MesTransmit();
				   printf("++++++++message finish sending++++++++++ \r\n");	
				  Transmit_time_flag = DISABLE;
			}
		
		 
	 }
}
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
static void can_delay(__IO u32 nCount)
{
	for(; nCount != 0; nCount--);
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

