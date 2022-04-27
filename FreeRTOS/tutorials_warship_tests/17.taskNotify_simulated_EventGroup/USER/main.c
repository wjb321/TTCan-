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


  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}


//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
  u8 key;
  while(1)
    {
      if(EventGroupTask_Handler!=NULL)
        {
          key=KEY_Scan(0);
          switch(key)
            {

            case KEY1_PRES:

              /*core*/

              xTaskNotify(EventGroupTask_Handler, EVENTBIT_1, eSetBits );  //eSetValueWithOverwrite
              // only in task function, set specified event bit to 1
              break;
            case KEY2_PRES:

              /*core*/

              xTaskNotify(EventGroupTask_Handler, EVENTBIT_2, eSetBits );
              break;
            }
        }
      vTaskDelay(10); //延时10ms，也就是10个时钟节拍
    }
}

//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
  u8 num =0;
  BaseType_t err, EventValue;
  static u8 finalFlag,  Event0Flag, Event1Flag,Event2Flag;
  uint32_t NotifyValue;
  while(1)
    {

      if(EventGroupTask_Handler!=NULL)
        {
          //wait for specified event bit
          //some task may need synchronze with more events, so this task should
          //wait and judge many event bits. so if the events bits are not set to
          //1,wait for it till it is ready.
          err =xTaskNotifyWait( (uint32_t) 0,
                                (uint32_t) ULONG_MAX,
                                (uint32_t*) &NotifyValue,
                                (TickType_t) portMAX_DELAY);
         // printf("err is %ld \r\n", err);

          if(err == pdPASS)
            {
              if(NotifyValue & EVENTBIT_0)
                {
                  Event0Flag = 1;
                }
              else if(NotifyValue & EVENTBIT_1)
                {
                  Event1Flag = 1;
                }
              else if(NotifyValue & EVENTBIT_2)
                {
                  Event2Flag = 1;
                }
            }
          finalFlag =  Event0Flag |(Event1Flag <<1) |(Event2Flag <<2) ;
						 printf("finalFlag is %#x\r\n",finalFlag);
          if(finalFlag == 0x07)
            {
              num++;
              Event0Flag = 0;
              Event1Flag = 0;
              Event2Flag = 0;
							LED1=!LED1;
              printf("notify group comes %d times\r\n ", num);
            }
         

          
        }
    }
}






