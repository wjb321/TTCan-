1. change the micro definition: #define configSUPPORT_STATIC_ALLOCATION 1 
then find out the errors say 2 functions are not defined: vApplicationGetIdleTaskMemory(), vApplicationGetTimerTaskMemory()


2. vApplicationGetIdleTaskMemory(): allocate memory for the idle task, vApplicationGetTimerTaskMemory() : allocate memory for timer taks
   when create a task, each task has TCb and task stack, both need memory, when using static method, these memories should allocated by the programmer.
   afer enable the 1. the allocation for the idle and timers need to be configured.
   
3. program the test code: 
   a, start task structure
   b, task1 and task2 structure
   c, realize the functions for task1 and task2
