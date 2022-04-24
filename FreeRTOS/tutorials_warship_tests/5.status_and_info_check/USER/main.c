#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

//Start Task handler definition
#define START_TASK_PRIO 1
#define START_STK_SIZE 128
void start_task( void * pvParameters );
TaskHandle_t StartTask_Handler;
StackType_t StartTaskStack[START_STK_SIZE]; 
StaticTask_t StartTaskTCB;


//Task 1
#define First_TASK_PRIO 2
#define First_STK_SIZE 128
void first_task( void * pvParameters );
TaskHandle_t FirstTask_Handler;
StackType_t FirstTaskStack[First_STK_SIZE]; 
StaticTask_t FirstTaskTCB;


//task 2
#define Second_TASK_PRIO 3
#define Second_STK_SIZE 70
void second_task( void * pvParameters );
TaskHandle_t SecondtTask_Handler;
StackType_t SecondTaskStack[Second_STK_SIZE]; 
StaticTask_t SecondTaskTCB;


//define idle task
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t  IdleTaskTCB;

//define timer task
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH];
static StaticTask_t  TimerTaskTCB;


//memory for idle task
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &IdleTaskTCB;
  *ppxIdleTaskStackBuffer = IdleTaskStack ;
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}




//memory for timer task
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &TimerTaskTCB;
  *ppxTimerTaskStackBuffer = TimerTaskStack ;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}



char bufferinfo[1000];
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
  delay_init();	    				//延时函数初始化
  uart_init(115200);					//初始化串口
  LED_Init();		  					//初始化LED


  // get from task.h when configSUPPORT_ALLOCATION 1
  // create StartTask, which return the taskhandler: TaskHandle_t
  StartTask_Handler = xTaskCreateStatic( (TaskFunction_t)  start_task,
                                  (char *        ) "start_task",
                                  (uint32_t      )  START_STK_SIZE,
                                  (void *        )  NULL,
                                  (UBaseType_t   )  START_TASK_PRIO,
                                  (StackType_t * )  StartTaskStack,
                                  (StaticTask_t *)  &StartTaskTCB );


   vTaskStartScheduler();          //开启任务调度
}


void start_task( void * pvParameters )
{ 
	
	  // task 1
    FirstTask_Handler = xTaskCreateStatic( (TaskFunction_t)  first_task,
                                  (char *        ) "first_task",
                                  (uint32_t      )  First_STK_SIZE,
                                  (void *        )  NULL,
                                  (UBaseType_t   )  First_TASK_PRIO,
                                  (StackType_t * )  FirstTaskStack,
                                  (StaticTask_t *)  &FirstTaskTCB );
																	
		// task 2 															
		SecondtTask_Handler = xTaskCreateStatic( (TaskFunction_t)  second_task,
                                  (char *        ) "second_task",
                                  (uint32_t      )  Second_STK_SIZE,
                                  (void *        )  NULL,
                                  (UBaseType_t   )  Second_TASK_PRIO,
                                  (StackType_t * )  SecondTaskStack,
                                  (StaticTask_t *)  &SecondTaskTCB );
																	
	  vTaskDelete(StartTask_Handler);
}


void first_task( void * pvParameters )
{   
	int task1_num = 0;
	while(1)
	{
		task1_num ++;
	  LED0 = !LED0;
	  //vTaskDelete(StartTask_Handler);
	 if( task1_num == 5)
	 {
		// vTaskDelete(SecondtTask_Handler);
		  printf("task 2 deleted \r\n");
	 }
	 printf("task 1 runs %d times \r\n", task1_num);
	 vTaskDelay(1000);
	}

}


void second_task( void * pvParameters )
{   
	  TaskHandle_t TestHandler;
	  u32 totalRunTime ;
	  UBaseType_t   Teststate;
   	UBaseType_t   StackMiniSize;
	  UBaseType_t   Priority;
	  UBaseType_t   ArraySize, x;
	  TaskStatus_t *StatusArray;
    Priority = 	uxTaskPriorityGet(SecondtTask_Handler);
	  printf("task priority: Priority = %d \r\n", Priority) ;
	  ArraySize = uxTaskGetNumberOfTasks();
	  StatusArray = pvPortMalloc(ArraySize * sizeof(TaskStatus_t));
	if(StatusArray != NULL)
	{  
    ArraySize = uxTaskGetSystemState(  (TaskStatus_t *) StatusArray,
                                			 (UBaseType_t   ) ArraySize,
 																			 (uint32_t *    ) &totalRunTime );		
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		for(x=0;x<ArraySize;x++)
		{
			//通过串口打印出获取到的系统任务的有关信息，比如任务名称、
			//任务优先级和任务编号。
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n",				
					StatusArray[x].pcTaskName,
					(int)StatusArray[x].uxCurrentPriority,
					(int)StatusArray[x].xTaskNumber);
			
		}
	}
    TestHandler = xTaskGetHandle("second_task"); //INCLUDE_xTaskGetIdleTaskHandle should be 1 in task.h
	  printf("TestHandle = %#x \r\n", TestHandler);
    printf("QueryTask = %#x \r\n", SecondtTask_Handler);
	
    Teststate = eTaskGetState( TestHandler );
	  printf("Teststate = %#x \r\n", Teststate); //this is zero, cause the value is enum type, so 0 means running
	
	  vTaskList(bufferinfo);
	  printf("bufferinfo is: %s \r\n ", bufferinfo);
    while(1)
		{
		// this is for debugging, the value is smaller, it overflow the stack chance will be higher. 
	  // when publish to public, then it should be removed
		StackMiniSize = uxTaskGetStackHighWaterMark( TestHandler ) ;
	  printf("stackminisize = %d \r\n",StackMiniSize );  // 128 - 77 = 51 then 51*4 = 204
		int task2_num = 0;
			task2_num ++;
			LED1 = !LED1;
			printf("task 2 runs %d times \r\n", task2_num);
			vTaskDelay(1000);
		}															
	 // vTaskDelete(StartTask_Handler);
}


