

same like counter_semaphore, but use LCD show the change
1. for notify function
but now use the function: 
NotifyValue=ulTaskNotifyGive(pdFALSE,portMAX_DELAY);
NotifyValue=ulTaskNotifyTake(pdFALSE,portMAX_DELAY);


2, for semaphore method:
err=xSemaphoreGive(CountSemaphore);//释放计数型信号量
semavalue=uxSemaphoreGetCount(CountSemaphore);

xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//等待数值信号量
semavalue=uxSemaphoreGetCount(CountSemaphore); 	//获取数值信号量值




