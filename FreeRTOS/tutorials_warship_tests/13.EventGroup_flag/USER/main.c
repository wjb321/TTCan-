#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "exti.h"
#include "beep.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"





#define START_TASK_PRIO			1
#define START_STK_SIZE 			256  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define EVENTSETBIT_TASK_PRIO	2
#define EVENTSETBIT_STK_SIZE 	256  
TaskHandle_t EventSetBit_Handler;
void eventsetbit_task(void *pvParameters);


#define EVENTGROUP_TASK_PRIO	3
#define EVENTGROUP_STK_SIZE 	256  
TaskHandle_t EventGroupTask_Handler;
void eventgroup_task(void *pvParameters);


#define EVENTQUERY_TASK_PRIO	4
#define EVENTQUERY_STK_SIZE 	256  
TaskHandle_t EventQueryTask_Handler;
void eventquery_task(void *pvParameters);



EventGroupHandle_t EventGroupHandler;	//事件标志组句柄

#define EVENTBIT_0	(1<<0)				//according to the location, left shift 0,1,2 length
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_0|EVENTBIT_1|EVENTBIT_2)



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				
	uart_init(115200);					
	LED_Init();		  					
	KEY_Init();							
	EXTIX_Init();						
	BEEP_Init();					
	my_mem_init(SRAMIN);            	//初始化内部内存池

	

    xTaskCreate((TaskFunction_t )start_task,         
                (const char*    )"start_task",        
                (uint16_t       )START_STK_SIZE,   
                (void*          )NULL,             
                (UBaseType_t    )START_TASK_PRIO,      
                (TaskHandle_t*  )&StartTask_Handler);             
    vTaskStartScheduler();          //start the task scheduler
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           
   //create event group in dynamic way
	 // when configUSE_16_BIT_TICKS = 1, eventGroup has 8 bits: bit0-bit7, when it is 0, then bit0-bit23
	 EventGroupHandler=xEventGroupCreate();
	
	
    xTaskCreate((TaskFunction_t )eventsetbit_task,             
                (const char*    )"eventsetbit_task",           
                (uint16_t       )EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&EventSetBit_Handler);   	
 
								
    xTaskCreate((TaskFunction_t )eventgroup_task,             
                (const char*    )"eventgroup_task",           
                (uint16_t       )EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&EventGroupTask_Handler);  
	
//		//create event query for event flag
//    xTaskCreate((TaskFunction_t )eventquery_task,             
//                (const char*    )"eventquery_task",           
//                (uint16_t       )EVENTQUERY_STK_SIZE,        
//                (void*          )NULL,                  
//                (UBaseType_t    )EVENTQUERY_TASK_PRIO,        
//                (TaskHandle_t*  )&EventQueryTask_Handler);    
    vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();          
}


//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
	u8 key;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			key=KEY_Scan(0);
			switch(key)
			{
				case KEY1_PRES:
					// only in task function, set specified event bit to 1
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
					break;
				case KEY2_PRES:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_2);
					break;	
			}
		}
        vTaskDelay(10); //延时10ms，也就是10个时钟节拍
	}
}

//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	u8 num;
	EventBits_t EventValue;
	while(1)
	{

		if(EventGroupHandler!=NULL)
		{
			//wait for specified event bit
			//some task may need synchronze with more events, so this task should
			//wait and judge many event bits. so if the events bits are not set to
			//1,wait for it till it is ready.
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		// the event group that is waiting for
										                 (EventBits_t			    )EVENTBIT_ALL,        // specify the bit that wait for
			                            
										                 (BaseType_t		    	)pdTRUE , //clearOnExit, if it is false, then it coverage the marker, so it is set to 1 all the time.
    /*
              			                 before exit this function, uxBitToWaitFor that configured will be reset to zero
			                               if any bit set to 1, then the values shows: just set fifth parameter to pdFALSE
			                                */			
										                 (BaseType_t			    )pdFALSE, //pdTRUE,             // 
								                     (TickType_t			    )portMAX_DELAY);	    // set the delay time
			printf("all bits set 1 and EventValue is %#x\r\n",EventValue);
			num++;
			LED1=!LED1;	
		}
		else
		{
			vTaskDelay(10); //延时10ms，也就是10个时钟节拍
		}
	}
}

////事件查询任务
//void eventquery_task(void *pvParameters)
//{	
//	u8 num=0;
//	EventBits_t NewValue,LastValue;
//	while(1)
//	{
//		if(EventGroupHandler!=NULL)
//		{
//			NewValue=xEventGroupGetBits(EventGroupHandler);	//get current eventgroup value
//			if(NewValue!=LastValue)
//			{
//				LastValue=NewValue;
//				printf("NewValue event group is:%d\r\n",NewValue);
//			}
//		}
//		num++;
//		if(num==10) 	//每500msLED0闪烁一次
//		{
//			num=0;
//			LED0=!LED0;	
//		}
//		vTaskDelay(50); //延时50ms，也就是50个时钟节拍
//	}
//}





