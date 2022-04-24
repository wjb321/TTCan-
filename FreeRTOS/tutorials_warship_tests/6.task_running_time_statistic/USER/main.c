#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"

//here is the configuration in the FreeRTOS 
//#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()  ConfigureTimeForRunTimeStats()//��ʱ��3�ṩʱ��ͳ�Ƶ�ʱ����Ƶ��Ϊ10K��������Ϊ100us
//configure in timer.c

#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);


#define TASK1_TASK_PRIO		2
#define TASK1_STK_SIZE 		128  
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);


#define TASK2_TASK_PRIO		3	
#define TASK2_STK_SIZE 		128  
TaskHandle_t Task2Task_Handler;
void task2_task(void *pvParameters);


#define RUNTIMESTATS_TASK_PRIO	4
#define RUNTIMESTATS_STK_SIZE 	128  
TaskHandle_t RunTimeStats_Handler;
void RunTimeStats_task(void *pvParameters);

char RunTimeInfo[400];		

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 
	delay_init();	    				
	uart_init(115200);				
	LED_Init();		  					
	KEY_Init();						
	
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();       
}

//start task
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       
 
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
 
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
	
	  xTaskCreate((TaskFunction_t )RunTimeStats_task,     
                (const char*    )"RunTimeStats_task",   
                (uint16_t       )RUNTIMESTATS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )RUNTIMESTATS_TASK_PRIO,
                (TaskHandle_t*  )&RunTimeStats_Handler); 
    vTaskDelete(StartTask_Handler); 
    taskEXIT_CRITICAL();            
}

//task1
void task1_task(void *pvParameters)
{
	u8 task1_num=0;
	while(1)
	{
		task1_num++;	//����ִ1�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
		LED0=!LED0;
    vTaskDelay(1000);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

//task2
void task2_task(void *pvParameters)
{
	u8 task2_num=0;

	while(1)
	{
		task2_num++;	//task2 ++, when comes to 255 then set
    LED1=!LED1;
    vTaskDelay(1000);                           //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

//RunTimeStats
void RunTimeStats_task(void *pvParameters)
{
	u8 key=0;
	while(1)
	{
		key=KEY_Scan(0);
		if(key==WKUP_PRES)
		{
			memset(RunTimeInfo,0,400);				//clear info buffer
			
			/*core part */
			vTaskGetRunTimeStats(RunTimeInfo);		//��ȡ��������ʱ����Ϣ
			printf("taskName\t\t\tRunning time\t percentage of time \r\n");
			printf("%s\r\n",RunTimeInfo);
		}
		vTaskDelay(10);                           	//��ʱ10ms��Ҳ����1000��ʱ�ӽ���	
	}
}



