1. there are some errors like: expected a ")", "}" etc, mainly is the conflicts between macro definition.
2. if there is the macro definition, if it is needed as the input of the function, the definition should be different name, and the input can use the macro defination when calling the function.
3. in for loop, the variable can be defined and initialized when coming to configuration and select c++ and click support c99 mode.


3.31.22

Achieved:
1) 
master: send system matrix 
slave: receive the matrix

2) 
master: send the reference message with the according to the SM cycle
slave: when receive the ref, its BC++, initialize its internal timer immediately(synchronized) and
       when timer isr comes check (BC_counter, SOS_counter) in received SM, and send the message according to the IDs

3) 
master: anytime when initializing it
slave: restart from BC0 again no matter previous BC comes.

4) 

slave: has same time slot(need to be changed to different time slots)
