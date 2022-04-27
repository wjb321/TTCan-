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
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
  delay_init();	    				//��ʱ������ʼ��
  uart_init(115200);					//��ʼ������
  LED_Init();		  					//��ʼ��LED
  KEY_Init();							//��ʼ������
  BEEP_Init();						//��ʼ��������
  my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��


  //������ʼ����
  xTaskCreate((TaskFunction_t )start_task,            //������
              (const char*    )"start_task",          //��������
              (uint16_t       )START_STK_SIZE,        //�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
              (TaskHandle_t*  )&StartTask_Handler);   //������
  vTaskStartScheduler();          //�����������
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

//�ͷż������ź���������
void TimerControl_Task(void *pvParameters)
{
  u8 key;
  BaseType_t err;
  //https://www.runoob.com/w3cnote/c-static-effect.html
  static u32 times; //static �ĵ�����������Ĭ�ϳ�ʼ��Ϊ 0��
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
      vTaskDelay(10);     //��ʱ10ms��Ҳ����10��ʱ�ӽ���
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
// vTaskDelay(1000);     //��ʱ10ms��Ҳ����10��ʱ�ӽ���
}