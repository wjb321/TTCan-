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
#include "queue.h"

//include this file
#include "semphr.h"
//deleted from usart isr
//	 //������з��ͽ��յ�������
//	if((USART_RX_STA&0x8000)&&(Message_Queue!=NULL))
//	{
//		xQueueSendFromISR(Message_Queue,USART_RX_BUF,&xHigherPriorityTaskWoken);//������з�������
//
//		USART_RX_STA=0;
//		memset(USART_RX_BUF,0,USART_REC_LEN);//������ݽ��ջ�����USART_RX_BUF,������һ�����ݽ���
//
//		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//�����Ҫ�Ļ�����һ�������л�
//	}


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		256
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define TASK1_TASK_PRIO		2
//�����ջ��С
#define TASK1_STK_SIZE 		256
//������
TaskHandle_t Task1Task_Handler;
//������
void task1_task(void *pvParameters);

//�������ȼ�
#define DATAPROCESS_TASK_PRIO 3
//�����ջ��С
#define DATAPROCESS_STK_SIZE  256
//������
TaskHandle_t Dataprocess_Handler;
//������
void Dataprocess_task(void *pvParameters);


//������Ϣ���е�����
#define KEYMSG_Q_NUM    1  		//������Ϣ���е�����  
#define MESSAGE_Q_NUM   4   	//�������ݵ���Ϣ���е����� 
QueueHandle_t Key_Queue;   		//����ֵ��Ϣ���о��
QueueHandle_t Message_Queue;	//��Ϣ���о��

//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]= {	WHITE, BLACK, BLUE,  BRED,
                        GRED,  GBLUE, RED,   MAGENTA,
                        GREEN, CYAN,  YELLOW,BROWN,
                        BRRED, GRAY
                      };

//������LCD����ʾ���յ��Ķ��е���Ϣ
//str: Ҫ��ʾ���ַ���(���յ�����Ϣ)
void disp_str(u8* str)
{
  LCD_Fill(5,230,110,245,WHITE);					//�������ʾ����
  LCD_ShowString(5,230,100,16,16,str);
}



//��ѯMessage_Queue�����е��ܶ���������ʣ���������
void check_msg_queue(void)
{
  u8 *p;
  u8 msgq_remain_size;	//��Ϣ����ʣ���С
  u8 msgq_total_size;     //��Ϣ�����ܴ�С

  taskENTER_CRITICAL();   //�����ٽ���
  msgq_remain_size=uxQueueSpacesAvailable(Message_Queue);//�õ�����ʣ���С
  msgq_total_size=uxQueueMessagesWaiting(Message_Queue)+uxQueueSpacesAvailable(Message_Queue);//�õ������ܴ�С���ܴ�С=ʹ��+ʣ��ġ�
  p=mymalloc(SRAMIN,20);	//�����ڴ�
  sprintf((char*)p,"Total Size:%d",msgq_total_size);	//��ʾDATA_Msg��Ϣ�����ܵĴ�С
  sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//��ʾDATA_Msgʣ���С

  myfree(SRAMIN,p);		//�ͷ��ڴ�
  taskEXIT_CRITICAL();    //�˳��ٽ���
}

/*define binary semaphorehandler*/
SemaphoreHandle_t SemaphoreBinary; 
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
  delay_init();	    				//��ʱ������ʼ��
  uart_init(115200);					//��ʼ������
  LED_Init();		  					//��ʼ��LED
  KEY_Init();							//��ʼ������
  BEEP_Init();						//��ʼ��������
  LCD_Init();							//��ʼ��LCD
  TIM2_Int_Init(5000,7200-1);			//��ʼ����ʱ��2������500ms
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

//��ʼ����������
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();           //�����ٽ���


  SemaphoreBinary =  xSemaphoreCreateBinary(); // create binarySemaphore
  if(SemaphoreBinary == NULL)
    {
      printf("create fail \r\n");
    }
  //����TASK1����
  xTaskCreate((TaskFunction_t )task1_task,
              (const char*    )"task1_task",
              (uint16_t       )TASK1_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )TASK1_TASK_PRIO,
              (TaskHandle_t*  )&Task1Task_Handler);
  //����TASK2����
  xTaskCreate((TaskFunction_t )Dataprocess_task,
              (const char*    )"Dataprocess_task",
              (uint16_t       )DATAPROCESS_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )DATAPROCESS_TASK_PRIO,
              (TaskHandle_t*  )&Dataprocess_Handler);
  vTaskDelete(StartTask_Handler); //ɾ����ʼ����
  taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void task1_task(void *pvParameters)
{
  u8 key;
  while(1)
    {
      key = KEY_Scan(0);
      if(key == WKUP_PRES)
        {
          if(SemaphoreBinary != NULL)
            { 
							/*core */
              xSemaphoreGive(SemaphoreBinary); // semd semaphore
            }
        }
      LED0=!LED0;
      printf("task1 is runnig \r\n") ;
      vTaskDelay(1000);                           //��ʱ10ms��Ҳ����10��ʱ�ӽ���
    }
}


//Keyprocess_task����
void Dataprocess_task(void *pvParameters)
{  
	//BaseType_t err;
  //u8 count = 0;
  while(1)
    {
      //count ++;
      if(SemaphoreBinary != NULL)
        {  
					/*core */
          xSemaphoreTake(SemaphoreBinary, portMAX_DELAY); // receive semaphore, second parameter: right here wait(wait till dead), wont come to next line, only if i press the WKUP_Press
				  //err =	xSemaphoreTake(SemaphoreBinary, 5000); // this means it will wait for 1s, if there is no semaphore, it will stop waitting, then come to next line
          printf("receive data: %s \r\n", USART_RX_BUF);
					memset(USART_RX_BUF, 0, USART_REC_LEN);// fresh the receve buff, otherwise the old data will still stored
					USART_RX_STA =0;
//					if(err == pdTRUE)
//					{
//						printf("running time is %d \r\n", count);
//					}
        }
      else
        {
          vTaskDelay(10);                           //��ʱ10ms��Ҳ����10��ʱ�ӽ���
        }
      LED1=!LED1;
      printf("task2 is runnig \r\n") ;
      vTaskDelay(1000);                           //��ʱ10ms��Ҳ����10��ʱ�ӽ���

    }

}



