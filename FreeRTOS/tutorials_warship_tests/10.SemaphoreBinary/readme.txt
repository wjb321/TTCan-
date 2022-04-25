 there are two parts done in semaphoreBinary
 1. task and task semaphore synchronize
 2. task and ISR(usart) semaphore synchronize
 
 the priority in usart, should configure higher than 4, otherwise, it will reach to nonmaskeable interrupt
 it will be some errors.