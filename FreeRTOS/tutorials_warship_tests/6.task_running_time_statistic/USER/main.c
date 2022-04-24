#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

//here is the configuration in the FreeRTOS 
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()  ConfigureTimeForRunTimeStats()//定时器3提供时间统计的时基，频率为10K，即周期为100us
//configure in timer.c

#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define TASK1_TASK_PRIO		2
#define TASK1_STK_SIZE 		128  
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);


#define TASK2_TASK_PRIO		3	
#define TASK2_STK_SIZE 		128  
TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);


#define RUNTIMESTATS_TASK_PRIO	4
#define RUNTIMESTATS_STK_SIZE 	128  
TaskHandle_t RunTimeStats_Handler;
void RunTimeStats_task(void *pvParameters);

char RunTimeInfo[400];		

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	delay_init();	    				
	uart_init(115200);				
	LED_Init();		  					
	KEY_Init();						
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();       
}

//start task
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       
 
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
 
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
	
	  xTaskCreate((TaskFunction_t )RunTimeStats_task,     
                (const char*    )"RunTimeStats_task",   
                (uint16_t       )RUNTIMESTATS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RUNTIMESTATS_TASK_PRIO,
                (TaskHandle_t*  )&RunTimeStats_Handler); 
    vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();            
}

//task1
void task1_task(void *pvParameters)
{
	u8 task1_num=0;
	while(1)
	{
		task1_num++;	//任务执1行次数加1 注意task1_num1加到255的时候会清零！！
		LED0=!LED0;
    vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}

//task2
void task2_task(void *pvParameters)
{
	u8 task2_num=0;

	while(1)
	{
		task2_num++;	//task2 ++, when comes to 255 then set
    LED1=!LED1;
    vTaskDelay(1000);                           //延时1s，也就是1000个时钟节拍	
	}
}

//RunTimeStats
void RunTimeStats_task(void *pvParameters)
{
	u8 key=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			memset(RunTimeInfo,0,400);				//clear info buffer
			
			/*core part */
			vTaskGetRunTimeStats(RunTimeInfo);		//获取任务运行时间信息
			printf("taskName\t\t\tRunning time\t percentage of time \r\n");
			printf("%s\r\n",RunTimeInfo);
		}
		vTaskDelay(10);                           	//延时10ms，也就是1000个时钟节拍	
	}
}



