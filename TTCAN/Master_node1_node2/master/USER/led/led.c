    
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
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_ResetBits(GPIOA,GPIO_Pin_0);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOC,GPIO_Pin_13); 						 //PE.5 ����� 
}