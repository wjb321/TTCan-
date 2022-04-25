Check two different timers:

AutoRerloadTimer

OneShotTimer

no delay function etc in the timercallback(), otherwise the timer will be useless

after configuring timerCallBack function, timer still does not work,
should do: 
1. xTimerStart(AutoRerloadTimer_Handler, 0);
2. xTimerStop( AutoRerloadTimer_Handler,0 );

same for OneShotTimer, but it does not have to xTimerStop(), cause it runs only once.