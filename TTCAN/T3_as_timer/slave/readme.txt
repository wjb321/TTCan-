
Problems:

do the loopback test, there are two different nodes A, B.
1) try these nodes with this code, A generates the data is what I want while B has different data in CAN_DATA6 and 
CAN_DATA7 parts, when there are CAN_DATA0-7



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

1.4.22

slave: checking the psa list and sm_BC0, triggered at each sos(initialize the timers differently), and make sure which message belongs to which node is going to send


4.4.22

slave: only T3 triggred when BC comes and SOS comes, according to the receiced SM, the message will be sent in the given slot. and now there is no receive node yet.
       Tobias mentioned due to all the nodes receives the SM, and the time is synchronized, so all the nodes know when it sends its message, so it only triggered in the its own 
	   message slot, there is no need for the SOS interrugp always.