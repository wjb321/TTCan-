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
/*core for ULONG_MAX*/
#include "limits.h"



#define START_TASK_PRIO			1
#define START_STK_SIZE 			128
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define MALLOC_TASK_PRIO	2
#define MALLOC_STK_SIZE 	128
TaskHandle_t MallocTask_Handler;
void Malloc_task(void *pvParameters);

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


  xTaskCreate((TaskFunction_t )Malloc_task,
              (const char*    )"Malloc_task",
              (uint16_t       )MALLOC_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )MALLOC_TASK_PRIO,
              (TaskHandle_t*  )&MallocTask_Handler);
  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}


//设置事件位的任务
void Malloc_task(void *pvParameters)
{
  u8 key;
  u8 *buffer;
  u8 i=0,timer = 0;
	u32 freemem;
  while(1)
    {
      key = KEY_Scan(0);
      switch(key)
        {
        case WKUP_PRES:
          buffer =pvPortMalloc( 30 );

          printf("the applied locaiton is %#x \r\n", (int)buffer);
          break;
        case KEY2_PRES:
          if(buffer!=NULL) vPortFree(buffer); // cleared, but actually the buffer variable should still have the address with space 0
          buffer = NULL;  //buffer should be cleared.
          printf("internal cashes freed location: %#x \r\n ", (int)buffer);
          break;
        case KEY1_PRES:
          if( buffer != NULL)
            {
              timer ++;
              sprintf((char*)buffer, "user timers = %d\r\n", timer);
              printf("buffer is: %s \r\n", buffer);
            }
          else
            {
              printf("buffer is useless \r\n");
            }
          break;
        }
				i++;
				freemem = xPortGetFreeHeapSize();
				if(i == 50)
				{
					i = 0;
					LED0 = !LED0;
					printf("the left heap size is: %d\r\n", freemem);
				}
      vTaskDelay(10); //延时10ms，也就是10个时钟节拍
    }
}






