#include "stm32f10x.h"
#include "systick.h" 
#include "usart.h" 
#include <stdlib.h>
#include <stdio.h>
#include "led.h"
#include "can_config.h"
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
static void can_delay(__IO u32 nCount);

int main(void)
{
	delay_init();
	USART_Configuration();
  CAN_Configuration();

	printf("\r\n");
	printf("*****************************************************************\r\n");
  printf("*                                                               *\r\n");
	printf("*  Thesis CAN 3 COM£¡^_^  *\r\n");
	printf("*                                                               *\r\n");
	printf("*****************************************************************\r\n");
  /* Infinite loop */
  while (1)
	{ 
	  if( CanFlag == ENABLE )
	  {
	  CanFlag = DISABLE;
		printf("CAN Receive Data from com16 mcu:\r\n");
		printf("CAN ID %x \r\n",CAN_ID);
		printf("CAN_DATA0 %x \r\n",Rx_DATA0);
		printf("CAN_DATA1 %x \r\n",Rx_DATA1);
		printf("CAN_DATA2 %x \r\n",Rx_DATA2);
		printf("CAN_DATA3 %x \r\n",Rx_DATA3);
		printf("CAN_DATA4 %x \r\n",Rx_DATA4);
		printf("CAN_DATA5 %x \r\n",Rx_DATA5);
		printf("CAN_DATA6 %x \r\n",Rx_DATA6);
		printf("CAN_DATA7 %x \r\n",Rx_DATA7);
	  }
	  /*CAN mode configuration CAN Transmit*/
		CanWriteData(0x3333); 
		can_delay(100);
    led_on_off();
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

