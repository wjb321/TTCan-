    
	#include "led.h"
	
	uint8_t Display;

		void led_on_off()
		{
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
