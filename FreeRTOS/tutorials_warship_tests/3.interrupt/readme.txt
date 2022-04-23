
test:
    when using function portDISABLE_INTERRUPTS() to disable the interrupt, the one smaller than the 
	configMAX_SYSCALL_INTERRUPT_PRIORITY priority will be enabled while bigger than the priority can
	not be enabled, it will keep working.
	so when disable the interrupt, timer3 works only, enable the interrupt then timer3 and timer5 they 
	both work. why???
	
	reason£ºin timer.c file, the priority of interrupts of timer3 and timer5 are configured as 3 and 5,
	so timer3 is still working when disable the interupt. just because it has no influence.


reference£ºSTM32F103 FreeRTOS manual.pdf 