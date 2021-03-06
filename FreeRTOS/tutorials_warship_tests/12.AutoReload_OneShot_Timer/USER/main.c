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
#include "timers.h"


#define START_TASK_PRIO			1
#define START_STK_SIZE 			256
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define TIMERCONTROL_TASK_PRIO		2
#define TIMERCONTROL_STK_SIZE 		256
TaskHandle_t TimerControl_Handler;
void TimerControl_Task(void *pvParameters);
void AutoReloadCallBack ( TimerHandle_t xTimer ); // cycle reload function
void OneShotCallBack ( TimerHandle_t xTimer ); // cycle reload function

TimerHandle_t AutoRerloadTimer_Handler; //  define 2 different timer, this cycle tiemr
TimerHandle_t OneShotTimer_Handler; // single time timer(one shot)
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

  /*in dynamic status*/
  AutoRerloadTimer_Handler = xTimerCreate((const char*) "AutoRerloadTimer_Handler",
                                          (TickType_t) 1000,
                                          (UBaseType_t) pdTRUE, // if it is cycle mode
                                          (void * )1,
                                          (TimerCallbackFunction_t) AutoReloadCallBack );



  OneShotTimer_Handler = xTimerCreate((const char*) "OneShotloadTimer_Handler",
                                      (TickType_t) 1000,
                                      (UBaseType_t) pdFALSE, // if it is cycle mode
                                      (void * )1,
                                      (TimerCallbackFunction_t) OneShotCallBack );


  xTaskCreate((TaskFunction_t )TimerControl_Task,
              (const char*    )"TimerControl_task",
              (uint16_t       )TIMERCONTROL_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )TIMERCONTROL_TASK_PRIO,
              (TaskHandle_t*  )&TimerControl_Handler);
  vTaskDelete(StartTask_Handler);
  taskEXIT_CRITICAL();
}

//释放计数型信号量任务函数
void TimerControl_Task(void *pvParameters)
{
  u8 key;
  BaseType_t err;
  //https://www.runoob.com/w3cnote/c-static-effect.html
  static u32 times; //static 的第三个作用是默认初始化为 0。
  while(1)
    {
      key = KEY_Scan(0);
      switch(key)
        {

        case WKUP_PRES:  // enable cycle timer
          //xTimerStart( xTimer, xTicksToWait ) xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

          err = xTimerStart(AutoRerloadTimer_Handler, 0);
          if(err == pdFAIL)
            {
              printf("AutoTimer starts failed \r\n");
            }
          else
            {
              printf("AutoTimer starts successfully \r\n");
            }
          break;

        case KEY1_PRES:
          err = xTimerStop( AutoRerloadTimer_Handler,0 );
          if(err == pdFAIL)
            {
              printf("AutoTimer stops failed \r\n");
            }
          else
            {
              printf("AutoTimer stops successfully \r\n");
            }
          break;

        case KEY0_PRES:  // enable cycle timer
          //xTimerStart( xTimer, xTicksToWait ) xTimerGenericCommand( ( xTimer ), tmrCOMMAND_START, ( xTaskGetTickCount() ), NULL, ( xTicksToWait ) )

          err = xTimerStart(OneShotTimer_Handler, 0);
          if(err == pdFAIL)
            {
              printf("OneShot starts failed \r\n");
            }
          else
            {
              printf("OneShot starts successfully \r\n");
            }
          break;

        case KEY2_PRES:
          err = xTimerStop( OneShotTimer_Handler,0 ); // oneShot does not have to turn off
          if(err == pdFAIL)
            {
              printf("OneShot stops failed \r\n");
            }
          else
            {
              printf("OneShot stops successfully \r\n");
            }
          break;
        }
      //printf("timercontrol task is running \r\n");
      //LED0=!LED0;
      vTaskDelay(10);     //延时10ms，也就是10个时钟节拍
    }
}

void AutoReloadCallBack ( TimerHandle_t xTimer ) // cycle reload function
{
  static u8 timers = 0;
  timers ++;
  LED1=!LED1;
  printf("AutoLoad timer is running %d \r\n", timers);
  //vTaskDelay(1000);    // timerCallBack function can not call any function which block the timer here, like vTaskDelay()
	                       // otherwise the timer has no meaning
}

void OneShotCallBack ( TimerHandle_t xTimer ) // cycle reload function
{
  static u8 timers = 0;
  timers ++;
  printf("OneShot timer is running %d \r\n", timers);
  BEEP= !BEEP;
  printf("timercontrol task is running \r\n");
// vTaskDelay(1000);     //延时10ms，也就是10个时钟节拍
}
