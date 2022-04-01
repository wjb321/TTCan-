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
extern uint16_t CAN_ID;
extern uint8_t Rx_DATA0,Rx_DATA1,Rx_DATA2,Rx_DATA3,Rx_DATA4,Rx_DATA5,Rx_DATA6,Rx_DATA7;
extern uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
extern uint8_t CanFlag; 
extern uint64_t time;
extern uint16_t SM_ID_Sent_flag; // from isr
extern uint16_t mes_send_flag;
extern uint16_t mes_send_flag;
extern uint8_t SOS_ISR_Count; 
extern uint8_t CanFlag; 
extern uint64_t ref_time;
extern uint16_t CAN_ID;
extern uint16_t  mes_id[][NumSlot];
extern uint16_t selected_ID;	
extern uint32_t SM_ID_Sent_Counter;
extern uint16_t  mes_id[TotNumBC][NumSlot];	
static void can_delay(__IO u32 nCount);
uint32_t i=0;
uint32_t matrix_send_time=0;
extern uint16_t NumBC;	 	

int main(void)
{
	delay_init();
	LED_Init();
	USART_Configuration();
  CAN_Configuration();

	printf("\r\n");
	printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
	printf("*  Thesis CAN 3 COM！^_^  *\r\n");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");
	/*SM sending time*/
  TIM3_Int_Init(1999, ENABLE, 7199);//10Khz的计数频率，计数到5000为500ms, 4999 for sm time, 14999 for BC time
	/*BC sending time*/
  TIM2_Int_Init(52000, DISABLE , 7199) ;// arr_sm is for sm sending, BC is for Basic cycle sending

  /* Infinite loop */
  while (1)
	{ 

    if(mes_send_flag == ENABLE)
		{
		 printf("master node sends NumBC =%d:\r\n", NumBC);
		 SendRef(MaskID_Ref, TotNumBC, NumSlot, SlotTime);
		 mes_send_flag = DISABLE;
		 if(NumBC >= TotNumBC){
	   NumBC = 0;}
		 
		}
		if (SM_ID_Sent_flag == ENABLE)
		{
			SendMatrix(MaskID_SM);
			printf("reference message is sent: %d \r\n", SM_ID_Sent_Counter);
			SM_ID_Sent_flag = DISABLE;
		}
//		else if (mes_send_flag == ENABLE)
//		{
//			mes_send_flag = DISABLE;
//			++SOS_ISR_Count; // start of slot isr times counter
//			if(SOS_ISR_Count >= NumSlot){
//				SOS_ISR_Count = 0;}
//			printf("SOS_ISR_Count is %x \r\n",SOS_ISR_Count);
//			selected_ID = mes_id[NumBC][SOS_ISR_Count];
//			printf("selected_ID is %x \r\n",selected_ID);
//		}
//		if( CanFlag == ENABLE )
//	  {
//	    CanFlag = DISABLE;
//			printf("reference message is sent: %lld \r\n", time);
//		  if(NumBC >= TotNumBC)
//			{
//				NumBC = 0;
//			}
//			//MatrixReiceive(); //get the matrix for all the slave nodes
//	  }
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

