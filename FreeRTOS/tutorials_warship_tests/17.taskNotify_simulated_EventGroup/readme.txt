1. origin 
basic test: eventgroup flag

3 bits: 000

when pressing one key, the corresponding bit will be set

the set bit API are these:
xEventGroupSetBits(EventGroupHandler,EVENTBIT_1); // configure in task function
Result=xEventGroupSetBitsFromISR(EventGroupHandler,EVENTBIT_0,&xHigherPriorityTaskWoken);  // configure in ISR, here it is located in exti.c


EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler, // which task
							   (EventBits_t			)EVENTBIT_ALL,       // which bits
			                   (BaseType_t		    )pdTRUE , 		    // clearonExit
							   (BaseType_t			)pdFALSE,           // all set or anyone set?
							   (TickType_t			)portMAX_DELAY);	
							   
							   
����������Ӧ�ı���λֵ��Ȼ��ȴ�����λ���������ã�Ҫô���֣�����ȫ��λ1ʱ�򣬽�������Ӧ�ı�־λ�����߷�������λ�õı�־λ��


2.simulates this group with taskNotify

most important part is the xTaskNotifyWait(), cause everytime the function is called, the bit will be reset, so here should give 
intermediate variables to store these values and then do the group test. check the core part code and the difference with EventGroup code