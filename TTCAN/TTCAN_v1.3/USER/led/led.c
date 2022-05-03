    
	#include "led.h"
	
	uint8_t Display1;

//	void led_on_off13()
//		{
//			if( Display1 ) 
//	   { 
//        /*====LED-ON=======*/
//	    GPIO_SetBits(GPIOC , GPIO_Pin_13);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//	    GPIO_ResetBits(GPIOC , GPIO_Pin_13);
//	  }
//	  Display1 = ~Display1;   	
//	  delay_ms(200);  /* delay 200ms */  
//   }
//		
//		uint8_t Display2;

//	void led_on_off14()
//		{
//			if( Display2 ) 
//	   { 
//        /*====LED-ON=======*/
//			GPIO_SetBits(GPIOC , GPIO_Pin_14);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//			GPIO_ResetBits(GPIOC , GPIO_Pin_14);
//	  }
//	  Display2 = ~Display2;   	
//	  delay_ms(200);  /* delay 200ms */  
//   }
//		
//	 	uint8_t Display3;

//	void led_on_off15()
//		{
//			if( Display3 ) 
//	   { 
//        /*====LED-ON=======*/
//	    GPIO_SetBits(GPIOA , GPIO_Pin_0);
//	    }  
//	  else 
//	  { 
//	    /*====LED-OFF=======*/ 
//	    GPIO_ResetBits(GPIOA, GPIO_Pin_0);
//	  }
//	  Display3 = ~Display3;   	
//	  delay_ms(100);  /* delay 200ms */  
//    }
		

		
	 
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;				 //LED0-->PB.5 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_0|GPIO_Pin_1);						 //PB.5 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 输出高 
}