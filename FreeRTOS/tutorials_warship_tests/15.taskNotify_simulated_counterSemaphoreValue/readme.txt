

same like counter_semaphore, but use LCD show the change
1. for notify function
but now use the function: 
NotifyValue=ulTaskNotifyGive(pdFALSE,portMAX_DELAY);
NotifyValue=ulTaskNotifyTake(pdFALSE,portMAX_DELAY);


2, for semaphore method:
err=xSemaphoreGive(CountSemaphore);//�ͷż������ź���
semavalue=uxSemaphoreGetCount(CountSemaphore);

xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//�ȴ���ֵ�ź���
semavalue=uxSemaphoreGetCount(CountSemaphore); 	//��ȡ��ֵ�ź���ֵ




