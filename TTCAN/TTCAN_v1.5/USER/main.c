#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
#include "stm32f10x.h"
#include "usart.h"
#include <stdio.h>
#include "can_config.h"
#include "led.h"
#include "nodes.h"
//#include <stdlib.h>
//#include "systick.h"
//#include "timer.h"
//#include "ttcan.h"
#define   MasterNode 0
#define   SlaveNode1 1
#define   SlaveNode2 2
int main(void)
{
  Delay_init();
  LED_Init();
  USART_Configuration();
 
  switch(SlaveNode2) //SlaveNode2
    {
    case 0:
      Node0();
      break;
    case 1:
      Node1();
      break;
    case 2:
      Node2();
      break;
    default:
      break;
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

