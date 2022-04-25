1, there are tasks low, middle, high.
low is running in the beginning.high request the cpu, cause hign priority then take the cpu,
while, low is still occupies the semaphore,high has to be in pending status.now middle comes
for cpu, then it takes the cpu, and running.high keeps waiting until it gets the semaphore.
so finally: high and low has same priority, while middle has the highest priority. now we say, 
it is the prioriry flipping.



2. for solve the priority flip problem.(mutex way)

when high request for the semaphore, then use mutexCreate to make semaphore, so when the semaphore
is ocuppied by low task, and the high task is in pending, the low priority will be taking to the same
priority as the high task, which reduce the chance that the middle task take the cpu and runs.

then the final status is high task is pending the semaphore till low task give its mutex semaphore.

3. there is also another mutexSemaphore: xSemaphoreTakeRecursive(); should check the differences with
   2.
