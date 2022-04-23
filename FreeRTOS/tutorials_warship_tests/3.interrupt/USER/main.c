#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"



#define START_TASK_PRIO			1
#define START_STK_SIZE 			256  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define INTERRUPT_TASK_PRIO		2
#define INTERRUPT_STK_SIZE 		256  
TaskHandle_t INTERRUPTTask_Handler;
void interrupt_task(void *p_arg);

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    				
	uart_init(115200);			
	LED_Init();		  					
	TIM3_Int_Init(10000-1,7200-1);		
	TIM5_Int_Init(10000-1,7200-1);	
	
	//create the start task
    xTaskCreate((TaskFunction_t )start_task,            
                (const char*    )"start_task",          
                (uint16_t       )START_STK_SIZE,        
                (void*          )NULL,                 
                (UBaseType_t    )START_TASK_PRIO,       
                (TaskHandle_t*  )&StartTask_Handler);             
    vTaskStartScheduler();          //开启任务调度
}

//start task
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           
   
    xTaskCreate((TaskFunction_t )interrupt_task,  		
                (const char*    )"interrupt_task", 			
                (uint16_t       )INTERRUPT_STK_SIZE,		
                (void*          )NULL,						
                (UBaseType_t    )INTERRUPT_TASK_PRIO,		
                (TaskHandle_t*  )&INTERRUPTTask_Handler); 	
	vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();            
}

//中断测试任务函数 
void interrupt_task(void *pvParameters)
{
	static u32 total_num=0;
    while(1)
    {
		  printf("second: %d \r\n",total_num);
		  total_num+=1;
		 if(total_num==5) 
		 {
			printf("disable interrupt...\r\n");
			portDISABLE_INTERRUPTS();				//interrupt disabled in rtos
			delay_xms(5000);						//delay, assume the system cpu is occupied
			printf("enable interrupt......\r\n");	//enable interrupt
			portENABLE_INTERRUPTS();
		 }
        LED0=~LED0;
        vTaskDelay(1000);
    }
} 

