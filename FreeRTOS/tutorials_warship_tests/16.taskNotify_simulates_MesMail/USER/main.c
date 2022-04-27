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
#include "limits.h"


#define START_TASK_PRIO		1
#define START_STK_SIZE 		256  
TaskHandle_t StartTask_Handler;
void start_task(void *pvParameters);

#define TASK1_TASK_PRIO		2
#define TASK1_STK_SIZE 		256  
TaskHandle_t Task1Task_Handler;
void task1_task(void *pvParameters);


#define KEYPROCESS_TASK_PRIO 3
#define KEYPROCESS_STK_SIZE  256 
TaskHandle_t Keyprocess_Handler;
void Keyprocess_task(void *pvParameters);


//������Ϣ���е�����
#define KEYMSG_Q_NUM    1  		//������Ϣ���е�����  


//LCDˢ��ʱʹ�õ���ɫ
int lcd_discolor[14]={	WHITE, BLACK, BLUE,  BRED,      
						GRED,  GBLUE, RED,   MAGENTA,       	 
						GREEN, CYAN,  YELLOW,BROWN, 			
						BRRED, GRAY };

//������LCD����ʾ���յ��Ķ��е���Ϣ
//str: Ҫ��ʾ���ַ���(���յ�����Ϣ)
void disp_str(u8* str)
{
	LCD_Fill(5,230,110,245,WHITE);					//�������ʾ����
	LCD_ShowString(5,230,100,16,16,str);
}

//����������
void freertos_load_main_ui(void)
{
	POINT_COLOR = RED;
	LCD_ShowString(10,10,200,16,16,"ATK STM32F103/407");	
	LCD_ShowString(10,30,200,16,16,"FreeRTOS Examp 13-1");
	LCD_ShowString(10,50,200,16,16,"Message Queue");
	LCD_ShowString(10,70,220,16,16,"KEY_UP:LED1 KEY0:Refresh LCD");
	LCD_ShowString(10,90,200,16,16,"KEY1:SendMsg KEY2:BEEP");
	
	POINT_COLOR = BLACK;
	LCD_DrawLine(0,107,239,107);		//����
	LCD_DrawLine(119,107,119,319);		//����
	LCD_DrawRectangle(125,110,234,314);	//������
	POINT_COLOR = RED;
	LCD_ShowString(0,130,120,16,16,"DATA_Msg Size:");
	LCD_ShowString(0,170,120,16,16,"DATA_Msg rema:");
	LCD_ShowString(0,210,100,16,16,"DATA_Msg:");
	POINT_COLOR = BLUE;
}



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
    freertos_load_main_ui();        	//������UI
	
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
	
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //����TASK2����
    xTaskCreate((TaskFunction_t )Keyprocess_task,     
                (const char*    )"keyprocess_task",   
                (uint16_t       )KEYPROCESS_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,
                (TaskHandle_t*  )&Keyprocess_Handler); 
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
void task1_task(void *pvParameters)
{
	u8 key,i=0;
    BaseType_t err;
	while(1)
	{
		key=KEY_Scan(0);        
        /* core */		
        if((Keyprocess_Handler!=NULL)&&(key))   	//��Ϣ����Key_Queue�����ɹ�,���Ұ���������
        {   
					   xTaskNotify( Keyprocess_Handler, (uint32_t)key,  eSetValueWithOverwrite);  // 3 = eSetValueWithOverwrite
//below 5 lines are from queue operation
//            err=xQueueSend(Key_Queue,&key,10);
//            if(err==errQUEUE_FULL)   	//���Ͱ���ֵ
//            {
//                printf("����Key_Queue���������ݷ���ʧ��!\r\n");
//            }
        }
        i++;
        if(i==50)
        {
            i=0;
            LED0=!LED0;
        }
        vTaskDelay(10);                           //��ʱ10ms��Ҳ����10��ʱ�ӽ���	
	}
}


//Keyprocess_task����
void Keyprocess_task(void *pvParameters)
{
	u8 num,key;
	BaseType_t err;
	uint32_t  NotifyValue;
	while(1)
	{
    
					  err = xTaskNotifyWait(0, ULONG_MAX, &NotifyValue, portMAX_DELAY); // limits.h should be included when using ULONG_MAX otherwise using 0xffffffffUL
					if(err == pdPASS)
					{  
						printf("the key value is: %d \r\n", NotifyValue);
						      switch((u8)NotifyValue)
                {
                    case WKUP_PRES:		//KEY_UP����LED1
                        LED1=!LED1;
                        break;
                    case KEY2_PRES:		//KEY2���Ʒ�����
                        BEEP=!BEEP;
                        break;
                    case KEY0_PRES:		//KEY0ˢ��LCD����
                        num++;
                        LCD_Fill(126,111,233,313,lcd_discolor[num%14]);
                        break;
                }
					}
	}
}


