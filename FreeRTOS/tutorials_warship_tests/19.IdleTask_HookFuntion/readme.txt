0. 
#define configUSE_TICKLESS_IDLE					1                       //1���õ͹���ticklessģʽ

1.
//low power mode in freeRTOSConfig.h
#define configPRE_SLEEP_PROCESSING     PreSleepProcessing
#define configPOST_SLEEP_PROCESSING    PostSleepProcessing

2.state PreSleepProcessing function in freeRTOSconfig.h:
extern void PreSleepProcessing(uint32_t xExpectedIdleTime);
extern void PostSleepProcessing(uint32_t xExpectedIdleTime);


3. commented out these two lines in freeRTOSconfig.h, which is used for time statistics interrupts time. and the interrupt is in a very short time(50Us).
   if do not close this interrupt, then every 50us the system will be waked up.
   so make sure close some fast interrupts or unnecessary interrupts.
   
   //#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()  ConfigureTimeForRunTimeStats()//��ʱ��3�ṩʱ��ͳ�Ƶ�ʱ����Ƶ��Ϊ10K��������Ϊ100us
//#define portGET_RUN_TIME_COUNTER_VALUE()		FreeRTOSRunTimeTicks	//��ȡʱ��ͳ��ʱ��ֵ



4.[check again need] if write printf() in PostSleepProcessing() or void PreSleepProcessing(uint32_t xExpectedIdleTime); it will make serial printing
   all the time, can not receicing command anymore. just because printf causes interrupts???
   
top part is for Tickless test, below is for hook 
5. #define configUSE_IDLE_HOOK						1                    //1��ʹ�ÿ��й��ӣ�0����ʹ��
6. define functions: beforeEnter and AfterExit...  void vApplicationIdleHook( void ) in main.c