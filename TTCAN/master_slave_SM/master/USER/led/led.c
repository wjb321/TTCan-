    
	#include "led.h"
	
	uint8_t Display;

		void led_on_off()
		{
			if( Display ) 
	   { 
        /*====LED-ON=======*/
	    GPIO_SetBits(GPIOC , GPIO_Pin_13);
			GPIO_SetBits(GPIOC , GPIO_Pin_14);
			//printf("lighting up led c13\r\n");
//	    GPIO_SetBits(GPIOC , GPIO_Pin_7);
//	    GPIO_SetBits(GPIOF , GPIO_Pin_8);
//	    GPIO_SetBits(GPIOF , GPIO_Pin_9);	 
	    }  
	  else 
	  { 
	    /*====LED-OFF=======*/ 
	    GPIO_ResetBits(GPIOC , GPIO_Pin_13);
			GPIO_ResetBits(GPIOC , GPIO_Pin_14);
			//printf("turing off led c13\r\n");
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_7);
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_8);
//	    GPIO_ResetBits(GPIOF , GPIO_Pin_9);		 
	  }
	  Display = ~Display;   	
	  delay_ms(200);  /* delay 200ms */  
  }
		
	
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 输出高 
}