#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
#include "FreeRTOS.h"
#include "task.h"

#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define KEY_TASK_PRIO		2
#define KEY_STK_SIZE 		128  
TaskHandle_t KeyTask_Handler;
void key_task(void *pvParameters);


#define TASK1_TASK_PRIO		3
#define TASK1_STK_SIZE 		128  
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);


#define TASK2_TASK_PRIO		4
#define TASK2_STK_SIZE 		128  
TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);


int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init();	    				
	uart_init(115200);					
	LED_Init();		  					
	KEY_Init();						
	EXTIX_Init();					
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,          
                (const char*    )"start_task",          
                (uint16_t       )START_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )START_TASK_PRIO,     
                (TaskHandle_t*  )&StartTask_Handler);              
    vTaskStartScheduler();       
}

//start task
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();          
//create key task
	xTaskCreate((TaskFunction_t )key_task,             
                (const char*    )"key_task",           
                (uint16_t       )KEY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )KEY_TASK_PRIO,        
                (TaskHandle_t*  )&KeyTask_Handler);  
    //task 1
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //task 2
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();          
}

//key
void key_task(void *pvParameters)
{
	u8 key;
	while(1)
	{
		key=KEY_Scan(0); // configure the key 
		switch(key)
		{
			case WKUP_PRES:
				vTaskSuspend(Task1Task_Handler);
				printf("suspend the task 1\r\n");
				break;
			case KEY1_PRES:
				vTaskResume(Task1Task_Handler);	
				printf("resume the task 1\r\n");
			case KEY2_PRES:
				vTaskSuspend(Task2Task_Handler);
				printf("suspend task 2\r\n");
				break;
		}
		vTaskDelay(10);	
	}
}

//task1
void task1_task(void *pvParameters)
{
	u8 task1_num=0;
	while(1)
	{
		task1_num++;
		LED0=!LED0;
		printf("task 1 has executed: %d times\r\n",task1_num);
        vTaskDelay(1000);                          
	}
}

//task2
void task2_task(void *pvParameters)
{
	u8 task2_num=0;
	while(1)
	{
		task2_num++;	
        LED1=!LED1;
		printf("task 2 has executed: %d times\r\n",task2_num);
        vTaskDelay(1000);                         
	}
}

