//relevant queueSend functio
//1,xQueueSend()
//2,xQueueSendToBack() 1,and 2 are same, both write to the tail of the Queue
//3,xQueueSendToFront() write to the queue head
//4,xQueueSendFromISR  
//5,xQueueSendToBackFromISR()  4,5 are same, both write to the tail in ISR: this configuration in usart.h/c 
//6,xQueueSendToFrontFromISR()  same like 3 but works in ISR
//7,xQueueOverwriteFrontISR()
//8,xQueueOverwrite() same like 7, but 7 works in ISR. when the Queue is full, it can overwrite the Queue 
//9,xQueueReceiveFromISR() is configured in timer.c isr part.

when using interrupt, make sure the priority is lower than the non-maskable interrupt in RTOS interrupt system.