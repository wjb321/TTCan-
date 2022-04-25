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
//	 //就向队列发送接收到的数据
//	if((USART_RX_STA&0x8000)&&(Message_Queue!=NULL))
//	{
//		xQueueSendFromISR(Message_Queue,USART_RX_BUF,&xHigherPriorityTaskWoken);//向队列中发送数据
//
//		USART_RX_STA=0;
//		memset(USART_RX_BUF,0,USART_REC_LEN);//清除数据接收缓冲区USART_RX_BUF,用于下一次数据接收
//
//		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);//如果需要的话进行一次任务切换
//	}


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		256
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小
#define TASK1_STK_SIZE 		256
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define DATAPROCESS_TASK_PRIO 3
//任务堆栈大小
#define DATAPROCESS_STK_SIZE  256
//任务句柄
TaskHandle_t Dataprocess_Handler;
//任务函数
void Dataprocess_task(void *pvParameters);


//按键消息队列的数量
#define KEYMSG_Q_NUM    1  		//按键消息队列的数量  
#define MESSAGE_Q_NUM   4   	//发送数据的消息队列的数量 
QueueHandle_t Key_Queue;   		//按键值消息队列句柄
QueueHandle_t Message_Queue;	//信息队列句柄

//LCD刷屏时使用的颜色
int lcd_discolor[14]= {	WHITE, BLACK, BLUE,  BRED,
                        GRED,  GBLUE, RED,   MAGENTA,
                        GREEN, CYAN,  YELLOW,BROWN,
                        BRRED, GRAY
                      };

//用于在LCD上显示接收到的队列的消息
//str: 要显示的字符串(接收到的消息)
void disp_str(u8* str)
{
  LCD_Fill(5,230,110,245,WHITE);					//先清除显示区域
  LCD_ShowString(5,230,100,16,16,str);
}



//查询Message_Queue队列中的总队列数量和剩余队列数量
void check_msg_queue(void)
{
  u8 *p;
  u8 msgq_remain_size;	//消息队列剩余大小
  u8 msgq_total_size;     //消息队列总大小

  taskENTER_CRITICAL();   //进入临界区
  msgq_remain_size=uxQueueSpacesAvailable(Message_Queue);//得到队列剩余大小
  msgq_total_size=uxQueueMessagesWaiting(Message_Queue)+uxQueueSpacesAvailable(Message_Queue);//得到队列总大小，总大小=使用+剩余的。
  p=mymalloc(SRAMIN,20);	//申请内存
  sprintf((char*)p,"Total Size:%d",msgq_total_size);	//显示DATA_Msg消息队列总的大小
  sprintf((char*)p,"Remain Size:%d",msgq_remain_size);	//显示DATA_Msg剩余大小

  myfree(SRAMIN,p);		//释放内存
  taskEXIT_CRITICAL();    //退出临界区
}

/*define binary semaphorehandler*/
SemaphoreHandle_t SemaphoreBinary; 
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
  delay_init();	    				//延时函数初始化
  uart_init(115200);					//初始化串口
  LED_Init();		  					//初始化LED
  KEY_Init();							//初始化按键
  BEEP_Init();						//初始化蜂鸣器
  LCD_Init();							//初始化LCD
  TIM2_Int_Init(5000,7200-1);			//初始化定时器2，周期500ms
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

//开始任务任务函数
void start_task(void *pvParameters)
{
  taskENTER_CRITICAL();           //进入临界区


  SemaphoreBinary =  xSemaphoreCreateBinary(); // create binarySemaphore
  if(SemaphoreBinary == NULL)
    {
      printf("create fail \r\n");
    }
  //创建TASK1任务
  xTaskCreate((TaskFunction_t )task1_task,
              (const char*    )"task1_task",
              (uint16_t       )TASK1_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )TASK1_TASK_PRIO,
              (TaskHandle_t*  )&Task1Task_Handler);
  //创建TASK2任务
  xTaskCreate((TaskFunction_t )Dataprocess_task,
              (const char*    )"Dataprocess_task",
              (uint16_t       )DATAPROCESS_STK_SIZE,
              (void*          )NULL,
              (UBaseType_t    )DATAPROCESS_TASK_PRIO,
              (TaskHandle_t*  )&Dataprocess_Handler);
  vTaskDelete(StartTask_Handler); //删除开始任务
  taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
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
      vTaskDelay(1000);                           //延时10ms，也就是10个时钟节拍
    }
}


//Keyprocess_task函数
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
          vTaskDelay(10);                           //延时10ms，也就是10个时钟节拍
        }
      LED1=!LED1;
      printf("task2 is runnig \r\n") ;
      vTaskDelay(1000);                           //延时10ms，也就是10个时钟节拍

    }

}



