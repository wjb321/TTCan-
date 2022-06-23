PWM test: 

create the breathing led

some issues:
LED0/1 in warship is on in 0 voltage and so when use pwm it should be some problems.
try to configure the pwm properly, choose pwm1 or pwm2? the difference is given.


here is the reference:https://blog.csdn.net/kinsno/article/details/46763243 

configuration: TIM4:PWM1:CH2:PB7(should configure this pin as pinout)


here use keil st-link online software simulation:
as for the configuration, it should be like this: https://blog.csdn.net/Leisure_ksj/article/details/105281185
briefly do the test of the program execution time by the st-link configuration.