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

//�ͷż������ź���������
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
    vTaskDelay(1000);     //��ʱ10ms��Ҳ����10��ʱ�ӽ���
    }
}

//��ȡ�������ź���������
void Middle_task(void *pvParameters)
{

  while(1)
    {
			printf("middle task is running \r\n");
      LED1=!LED1;
      vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}



//��ȡ�������ź���������
void High_task(void *pvParameters)
{

  while(1)
    { 
			printf("high task is Pend sem \r\n");
			xSemaphoreTake(MutexSemaphore, portMAX_DELAY);
			printf("high task is running \r\n");
			xSemaphoreGive(MutexSemaphore);
      //BEEP=!BEEP;
      vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}


