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

int lcd_discolor[14]= {	WHITE, BLACK, BLUE,  BRED,
                        GRED,  GBLUE, RED,   MAGENTA,
                        GREEN, CYAN,  YELLOW,BROWN,
                        BRRED, GRAY
                      };


int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
  delay_init();	    				//��ʱ������ʼ��
  uart_init(115200);					//��ʼ������
  LED_Init();		  					//��ʼ��LED
  KEY_Init();							//��ʼ������
  BEEP_Init();						//��ʼ��������
  LCD_Init();							//��ʼ��LCD
  my_mem_init(SRAMIN);            	//��ʼ���ڲ��ڴ��

  POINT_COLOR = RED;
  LCD_ShowString(30,10,200,16,16,"ATK STM32F103/407");
  LCD_ShowString(30,30,200,16,16,"FreeRTOS Examp 18-2");
  LCD_ShowString(30,50,200,16,16,"Task Notify Count Sem");
  LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
  LCD_ShowString(30,90,200,16,16,"2016/11/25");

  POINT_COLOR = BLACK;
  LCD_DrawRectangle(5,110,234,314);
  LCD_DrawLine(5,130,234,130);
  POINT_COLOR = RED;
  LCD_ShowString(54,111,200,16,16,"Notify Value:   0");
  POINT_COLOR = BLUE;

  //������ʼ����
  xTaskCreate((TaskFunction_t )start_task,            //������
              (const char*    )"start_task",          //��������
              (uint16_t       )START_STK_SIZE,        //�����ջ��С
              (void*          )NULL,                  //���ݸ��������Ĳ���
              (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
              (TaskHandle_t*  )&StartTask_Handler);   //������
  vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();           //�����ٽ���

  // free semaphore
  xTaskCreate((TaskFunction_t )SemapGive_task,
              (const char*    )"semapgive_task",
              (uint16_t       )SEMAPGIVE_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )SEMAPGIVE_TASK_PRIO,
              (TaskHandle_t*  )&SemapGiveTask_Handler);

  //take semaphore
  xTaskCreate((TaskFunction_t )SemapTake_task,
              (const char*    )"semaptake_task",
              (uint16_t       )SEMAPTAKE_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )SEMAPTAKE_TASK_PRIO,
              (TaskHandle_t*  )&SemapTakeTask_Handler);
  vTaskDelete(StartTask_Handler); //ɾ����ʼ����
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�ͷż������ź���������
void SemapGive_task(void *pvParameters)
{
  u8 key,i=0;
  while(1)
    {
      key=KEY_Scan(0);           	//ɨ�谴��
      if(SemapTakeTask_Handler!=NULL)
        {
          switch(key)
            {
            case WKUP_PRES:

              /*core*/
              xTaskNotifyGive(SemapTakeTask_Handler);//��������֪ͨ
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
  uint32_t NotifyValue;
  while(1)
    {
      /* core */
      NotifyValue=ulTaskNotifyTake(pdFALSE,portMAX_DELAY);//��ȡ����֪ͨ
      num++;
      LCD_ShowxNum(166,111,NotifyValue-1,3,16,0);         //��ʾ��ǰ����ֵ֪ͨ
      LCD_Fill(6,131,233,313,lcd_discolor[num%14]);   	//ˢ��
      LED1=!LED1;
      vTaskDelay(1000);                               	//��ʱ1s��Ҳ����1000��ʱ�ӽ���
    }
}



