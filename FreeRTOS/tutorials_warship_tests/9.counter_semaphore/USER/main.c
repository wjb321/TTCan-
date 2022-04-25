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


SemaphoreHandle_t CountSemaphore;//�������ź���



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

//�ͷż������ź���������
void SemapGive_task(void *pvParameters)
{
  u8 key,i=0;
  u8 semavalue;
  BaseType_t err;
  while(1)
    {
      key=KEY_Scan(0);           	//ɨ�谴��
      if(CountSemaphore!=NULL)  	//�������ź��������ɹ�
        {
          switch(key)
            {
            case WKUP_PRES:
              err=xSemaphoreGive(CountSemaphore);//�ͷż������ź���
              if(err==pdFALSE)
                {
                  printf("semaphoreGive failed!!!\r\n");
                }
              semavalue=uxSemaphoreGetCount(CountSemaphore);	//��ȡ�������ź���ֵ
              break;
            }
        }
      i++;
      if(i==50)
        {
          i=0;
          LED0=!LED0;
        }
      vTaskDelay(10);     //��ʱ10ms��Ҳ����10��ʱ�ӽ���
    }
}

//��ȡ�������ź���������
void SemapTake_task(void *pvParameters)
{
  u8 num;
  u8 semavalue;
  while(1)
    {
      xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//�ȴ���ֵ�ź���
      num++;
      semavalue=uxSemaphoreGetCount(CountSemaphore); 	//��ȡ��ֵ�ź���ֵ
      printf("semaphoreValue is %d!\r\n",semavalue);
      LED1=!LED1;
      vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}


