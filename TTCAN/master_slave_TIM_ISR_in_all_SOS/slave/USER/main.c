#include "stm32f10x.h"
#include "systick.h" 
#include "usart.h" 
#include <stdlib.h>
#include <stdio.h>
#include "led.h"
#include "can_config.h"
#include "timer.h"
#include "ttcan.h"
#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/
extern uint8_t Rx_DATA0,Rx_DATA1,Rx_DATA2,Rx_DATA3,Rx_DATA4,Rx_DATA5,Rx_DATA6,Rx_DATA7;
extern uint16_t Slot_ISR_Timer, mes_id[TotNumBC][NumSlot];
extern uint8_t CanRefFlag, CanSMFlag; 
extern uint16_t NumBC;
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
	TIM3_Int_Init(3500,7199,0);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms, 9999: 1 s 
  /* Infinite loop */
	int p =0; 
  while (1)
	{  
		//test for timer variables
//    uint16_t variables[16] = {500,500,500,500,500,500,500,500,8000,8000,15000,20000,25000,35000,45000,53000};
//		 if(SOS_ISR_flag == ENABLE )
//		 { 
//			 
//			 	LEDA0 = !LEDA0;
//			
//				TIM3_Int_Init(variables[p],7199,1); // this gets from timerset()
//        p++; 
//						 

//			
//			 SOS_ISR_flag = DISABLE;
//			}
//		 
    
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
			TimerSet();	
      TimerArray_p = 0;			
			TIM3_Int_Init(TimerArray[0],7199,1); // this gets from timerset()
			printf("TimerArray[i]=: %d \r\n", TimerArray[0]);	
      ++NumBC;			
			if(NumBC > TotNumBC){
	    NumBC = 0;}
			printf("Reference ID is sent: %#x \r\n", CANSM_ID);	
  	  SOS_ISR_Count = 0 ;			
	   }
		
		 /*SOS triggered internal messages*/
		 if(SOS_ISR_flag == ENABLE )
		 {  
			  SOS_ISR_Count++;
				LEDA0 = !LEDA0;
			  TimerArray_p ++;
				TIM3_Int_Init(TimerArray[TimerArray_p],7199,1); // this gets from timerset()
			  printf("TimerArray_p =: %d  in  sosisr \r\n", TimerArray_p);	
			  if(SOS_ISR_Count > NumSlot-2){
				TIM_Cmd(TIM3, 0); 
				}
			 SOS_ISR_flag = DISABLE;
			 
			if(NumBC == 0){
				NumBC = 1;
			}
			// �������ڴ˴�ͨ����Ӧ��Reiceived mes �е�ֵ���������������жϴӶ���������������һ��node,Ȼ����в�����
			printf("this mes id should be sent: NumBC = %d SOS_ISR_Count=%d  %#x \r\n", NumBC-1, SOS_ISR_Count, Received_mes_id[NumBC-1 ][SOS_ISR_Count]);
			
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

