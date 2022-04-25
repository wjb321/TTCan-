#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "malloc.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#define START_TASK_PRIO			1
#define START_STK_SIZE 			256
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define LOW_TASK_PRIO		2
#define LOW_STK_SIZE 		256
TaskHandle_t LowTask_Handler;
void Low_task(void *pvParameters);


#define MIDDLE_TASK_PRIO 	3
#define MIDDLE_STK_SIZE  	256
TaskHandle_t MiddleTask_Handler;
void Middle_task(void *pvParameters);


#define HIGH_TASK_PRIO 	4
#define HIGH_STK_SIZE  	256
TaskHandle_t HighTask_Handler;
void High_task(void *pvParameters);

SemaphoreHandle_t MutexSemaphore;//change from binarySemaphore to mutexSemaphore



int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
  delay_init();	    				//延时函数初始化
  uart_init(115200);					//初始化串口
  LED_Init();		  					//初始化LED
  KEY_Init();							//初始化按键
  BEEP_Init();						//初始化蜂鸣器
  my_mem_init(SRAMIN);            	//初始化内部内存池


  //创建开始任务
  xTaskCreate((TaskFunction_t )start_task,            //任务函数
              (const char*    )"start_task",          //任务名称
              (uint16_t       )START_STK_SIZE,        //任务堆栈大小
              (void*          )NULL,                  //传递给任务函数的参数
              (UBaseType_t    )START_TASK_PRIO,       //任务优先级
              (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
  vTaskStartScheduler();          //开启任务调度
}

//start task
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();
  MutexSemaphore = xSemaphoreCreateMutex(); // when creating mutexSemaphore, it Give(free) once already, so no need to xSemaphoreGive() anymore
//	if(MutexSemaphore != NULL)
//  {
//		xSemaphoreGive(MutexSemaphore);
//		printf("semaphore create successful \r\n");
//	}	
if(MutexSemaphore == NULL)
	{
		printf("semaphore create fails \r\n");
	}
  //Create the count semaphore
  //CountSemaphore=xSemaphoreCreateCounting(255,0);
	
	//Low
  xTaskCreate((TaskFunction_t )Low_task,
              (const char*    )"Low_task",
              (uint16_t       )LOW_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )LOW_TASK_PRIO,
              (TaskHandle_t*  )&LowTask_Handler);
 
	//Middle
  xTaskCreate((TaskFunction_t )Middle_task,
              (const char*    )"semaptake_task",
              (uint16_t       )MIDDLE_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )MIDDLE_TASK_PRIO,
              (TaskHandle_t*  )&MiddleTask_Handler);
							
								//Middle
  xTaskCreate((TaskFunction_t )High_task,
              (const char*    )"semaptake_task",
              (uint16_t       )HIGH_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )HIGH_TASK_PRIO,
              (TaskHandle_t*  )&HighTask_Handler);
  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}

//释放计数型信号量任务函数
void Low_task(void *pvParameters)
{
 static u32 times;
  while(1)
    {
    xSemaphoreTake(MutexSemaphore, portMAX_DELAY);
		printf("low task is running \r\n");
			
	  //modelling that low priority task is occuping the xSemaphore
	  for(times=0; times <2000000; times ++) // low priority is always taking the binary semaphore, high priority can not take it during this 
			{                                     // for loop so this make the priority flip
				taskYIELD(); // can be replaced by Delay_us();
				//Delay_us(2);
			}
		xSemaphoreGive(MutexSemaphore); // when the while finished, then free the BinarySemaphore, then high can take the resource and running
		LED0=!LED0;
    vTaskDelay(1000);     //延时10ms，也就是10个时钟节拍
    }
}

//获取计数型信号量任务函数
void Middle_task(void *pvParameters)
{

  while(1)
    {
			printf("middle task is running \r\n");
      LED1=!LED1;
      vTaskDelay(1000);                               //延时1s，也就是1000个时钟节拍
    }
}



//获取计数型信号量任务函数
void High_task(void *pvParameters)
{

  while(1)
    { 
			printf("high task is Pend sem \r\n");
			xSemaphoreTake(MutexSemaphore, portMAX_DELAY);
			printf("high task is running \r\n");
			xSemaphoreGive(MutexSemaphore);
      //BEEP=!BEEP;
      vTaskDelay(1000);                               //延时1s，也就是1000个时钟节拍
    }
}


