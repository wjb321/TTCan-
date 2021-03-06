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


#define SEMAPGIVE_TASK_PRIO		2
#define SEMAPGIVE_STK_SIZE 		256
TaskHandle_t SemapGiveTask_Handler;
void SemapGive_task(void *pvParameters);


#define SEMAPTAKE_TASK_PRIO 	3
#define SEMAPTAKE_STK_SIZE  	256
TaskHandle_t SemapTakeTask_Handler;
void SemapTake_task(void *pvParameters);


SemaphoreHandle_t CountSemaphore;//计数型信号量



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

  //Create the count semaphore
  CountSemaphore=xSemaphoreCreateCounting(255,0);
  xTaskCreate((TaskFunction_t )SemapGive_task,
              (const char*    )"semapgive_task",
              (uint16_t       )SEMAPGIVE_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )SEMAPGIVE_TASK_PRIO,
              (TaskHandle_t*  )&SemapGiveTask_Handler);

  xTaskCreate((TaskFunction_t )SemapTake_task,
              (const char*    )"semaptake_task",
              (uint16_t       )SEMAPTAKE_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )SEMAPTAKE_TASK_PRIO,
              (TaskHandle_t*  )&SemapTakeTask_Handler);
  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}

//释放计数型信号量任务函数
void SemapGive_task(void *pvParameters)
{
  u8 key,i=0;
  u8 semavalue;
  BaseType_t err;
  while(1)
    {
      key=KEY_Scan(0);           	//扫描按键
      if(CountSemaphore!=NULL)  	//计数型信号量创建成功
        {
          switch(key)
            {
            case WKUP_PRES:
              err=xSemaphoreGive(CountSemaphore);//释放计数型信号量
              if(err==pdFALSE)
                {
                  printf("semaphoreGive failed!!!\r\n");
                }
              semavalue=uxSemaphoreGetCount(CountSemaphore);	//获取计数型信号量值
              break;
            }
        }
      i++;
      if(i==50)
        {
          i=0;
          LED0=!LED0;
        }
      vTaskDelay(10);     //延时10ms，也就是10个时钟节拍
    }
}

//获取计数型信号量任务函数
void SemapTake_task(void *pvParameters)
{
  u8 num;
  u8 semavalue;
  while(1)
    {
      xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//等待数值信号量
      num++;
      semavalue=uxSemaphoreGetCount(CountSemaphore); 	//获取数值信号量值
      printf("semaphoreValue is %d!\r\n",semavalue);
      LED1=!LED1;
      vTaskDelay(1000);                               //延时1s，也就是1000个时钟节拍
    }
}



