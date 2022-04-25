there are tasks low, middle, high.

low is running in the beginning

high request the cpu, cause hign priority then take the cpu, while, low is still occupies the semaphore,
high has to be in pending status

now middle comes for cpu, then it takes the cpu, and running.

high keeps waiting until it gets the semaphore.

so finally: high and low has same priority, while middle has the highest priority. 
now we say, it is the prioriry flipping
there are the differences between binarySemaphore and mutex:https://blog.csdn.net/wanruoqingkong/article/details/36006921
