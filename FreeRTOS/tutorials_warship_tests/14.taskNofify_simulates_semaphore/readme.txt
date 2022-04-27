1.send messages: 

#define LED1ON	1
#define LED1OFF	2
#define BEEPON	3
#define BEEPOFF	4

2.by using serial, where the function is configured like:
vTaskNotifyGiveFromISR(DataProcess_Handler,&xHigherPriorityTaskWoken);	//send notify


3.and the serial buffer will be read by uing the function in the task:
NotifyValue=ulTaskNotifyTake(pdTRUE,portMAX_DELAY);	//take notify


4.then the experimental phenomenon is
led1 on and off, beep on and off.


5.the main idea is: using message notify function simulates message semaphore