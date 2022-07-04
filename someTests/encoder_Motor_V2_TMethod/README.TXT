做出了T方法测速，但是有跳变，在按键模拟不同速度的时候。

there is the T method for speed test, unfortunately, when pressing
key to simulate celerate, decelerate etc, the data some are unexpected jumps,
later will think about removing the jumps:

29.06.22
1, debouncing: delay method, interrupt method

2, impulse filters?

3, ...

4.7.22
Tmethod now can ignore while loop, only 
interrupts for encoder counter and another 
timer for counting the number of encoder interrupts.

probally can try kalman filter to remove the jumps, and other 
unexpected impulse.