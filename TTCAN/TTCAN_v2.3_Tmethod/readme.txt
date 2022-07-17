
Problems:

do the loopback test, there are two different nodes A, B.
  1) try these nodes with this code, A generates the data is what I want while B has different data in CAN_DATA6 and
    CAN_DATA7 parts, when there are CAN_DATA0-7

    对于节点的问题：

    1） 当节点接受到ref小时时候，就会BC ++;
    因为作为基本周期的标志。但是对于具体消息的发送应该考虑是从BC=0 开始进行的。
        所以应该进行BC-1，来进行BC0的操作。但是为了防止BC=0 时候，会产生BC-	1 = -1的情况.就将其调整到0.

            2)  当时想的是如此，使用两个定时器：T3 和T2。
    T3： 用于产生窗口，即在每个既定的窗口开始处进行中断并初始化， 根据接收到的系统矩阵，得到不同的窗口大小（因为不同的消息发送时间不同）。
    T2： 想着消息的发送中的transmission time 代表发送的时间，那么用T2来进行对应系统矩阵的ID进行给定时间的持续发送。即在T3到达中断时候，对T3 和T2 同时进行初始化。
    T2 开始的时候就是对应列表中的ID的发送，当中断时候，对应的ID就停止发送。但是经过考虑是不对的。因为每个对应的窗口只需要发送一次给定的window下的ID值，transmission
    time 代表的是消息在发送后在CAN总线上的持续时间。换言之不需要T２的参与，因为持续的时间是有发送的消息本身决定的。T２可以代表某个消息的“持续时间”，即可以不完全占满
    给定的window，当然最多占完整个window，但是这里出现一个问题：如果T２相比T３在给定的window下太短，那么T２有可能发生两次及两次以上的中断，那么可能会将对应的ID发送多次，
    这并不满足对应消息的周期特性。

    所以应该是这种方案：

    T３定时器，在得到ｒｅｆ之后，就开始初始化定时器，定时器的时间根据不同列的slot长度进行调整：即在每次ＩＳＲ的时候进行一次初始化，那么换言之：通过这种方式就得到对应的exclusive　window。
    那么在对应的初始化时候，就对相应的消息进行发送。然后T３中断到来（本时间窗口截止），进行新的T３窗口初始化，此时又进行对应窗口的ID的发送，直到最后的消息发送完毕。



    3.31.22

Achieved:
    1)
master:
    send system matrix
slave:
    receive the matrix

    2)
master:
    send the reference message with the according to the SM cycle
slave:
    when receive the ref, its BC++, initialize its internal timer immediately(synchronized) and
    when timer isr comes check (BC_counter, SOS_counter) in received SM, and send the message according to the IDs

    3)
master:
    anytime when initializing it
slave:
    restart from BC0 again no matter previous BC comes.

    4)

slave:
    has same time slot(need to be changed to different time slots)

    1.4.22

slave:
    checking the psa list and sm_BC0, triggered at each sos(initialize the timers differently), and make sure which message belongs to which node is going to send


    4.4.22

slave:
    only T3 triggred when BC comes and SOS comes, according to the receiced SM, the message will be sent in the given slot. and now there is no receive node yet.
    Tobias mentioned due to all the nodes receives the SM, and the time is synchronized, so all the nodes know when it sends its message, so it only triggered in the its own
    message slot, there is no need for the SOS interrugp always

      8.4.22

slave:
      T3 can rceive SM, and according its node message list, checking the location of the messsages in the SM, and store the location and time.
	  
	
13.04.22 

master: send matrix and BC as the synchronize signal.
slave: receive the matrix and according to the SM and its own messages list, check the the location of its own IDs in the SM, then send the required IDs according the time.
       the timer will be set according to the location. for example, the timer list is [3000, 8000, 7000], and messages location is [a, 0 ,b],then the timer should be set[0, 8000],
       and in different situations, send the respective messages
	   
01.05.22

ST link uploading code, sometime does not work, should go to this steps: -> options for Target -> debug setting -> debug -> delete the check mark of shearable of ST-linker

02.05. 22

ttcan.c file has configurations for slaveNode1 and slaveNode2, when do the updating and delete the brace by mistake and added in a wrong place. find the bug for a long time.
but it is not convinent when change to different node, i have to come to ttcan.c to configure again. so better way is using a interface then can change to any node.

04.05.22 

a.
solve the bug: when there is only one message after ref like:
slave node2 : 0x01 0x123 0x122 0x121 0 in all the BC, so there is no need to enable timer. it is only enable when finalTimerValue >1

b. 
when changing the matrix: ->> ttcan.h  ->> configure the NumSlot ->> change the matrix in ttcan.c:  mes_id and  is_exclusive_window arrays ->> main.c
switch MasterNode, uploading, switch SlaveNode1/2 uploading.

c.
be careful when uploading to different nodes, switch should be changed repestively!!!

d.
create sm with only one exclusive window.and the values are always same. do the test, the BC timer set 10ms can make sure the all the values cam be safely
sent. if it is faster than 11 ms, some BCs, message can not be sent. checke node0 -> timer2: configure with 110.

f. 
by setting one byte++, setting timer with 170, 7999, there is no data missing. so it should be 17 ms, then the frequency can be 60hz, means 60 samples per second. still need think
how to increase the frequency.

06.05.22
think about  MesTimesInBC = TimerISR(); part how to update, first tempMesLocation is an array to store the nodes messages location in receiced_ID, this part is fixed, so do not need to call 
everytime when BC comes.

8.5.22
try seperate TimerISR into 2 parts, which can save the time. but try the function seems does not work. now first try other parts.

9.5.22
after multiple tests, find the timer for BC can be configured as 16, with 1/ 1.7 = 600HZ, and the messages can be sent and received smoothly.

14.5.22
speed node send speed to ttcan node1(com15) and then send speed info via ttcan to  ttcan node2(3) use the speed to get response

now: ttcan sends speed change info and speed.

15.5.22 
according to different ids, call different functions to send, this different functions are different sensor messages.

16.5.22
pb3, 4 after reset, the main function are jtdo and jntrst modes, when used as noral io, should do the pinremap function.

24.5.22
after checking and debugging, there are some big updates:
1) send speed to node1, and everytime when the bc comes the send the coressponding speed info to node2, only according to the difference of speed, check
it is block or just normal decelerate.

2) the system matrix can be changed randomly, one can send every BC and another one can receice.


10.06.22
USART sends from warship board, and there is a problem:
the speed change in a proper range it regards as deceleration, then remeber: CAN bus can only send data 0-0xff, so maximum is 255, so for m/s is fast enough. but warship board send the speed info is 
1500, which is far higher than the expected value. there are two options: seperate the data into 2 parts, and CAN transmits the data  by data0 and data2. or send relative small value(fair enough)

12.06.22
here add some threshold by checking the speed, and according to the speed info, can fast detect the state of the car.

6.7.22
尝试将Tim2的中断服务函数放在Tim.c中，然后其他的依然保留在原函数中看效果如何。（没变化）
尝试了node2测试单一编码器代码，正常工作。所以应该是V2.0中的代码配置有问题导致其数值不停的变化。
混烧之后，现在v2的代码可以获取编码器的准确值，然后可以稳定地得到对应的速度值， 明天再进行一次测试。
明天： 测试完之后，利用获取的速度值，然后node2 发出速度， 然后node1 接收，工作后，通过将代码只执行一次，再测试其速度，以及time constraint。
the problem is found： all the nodes for counting the frequences should be common GND, otherwise the values will jump.

7.7.22
now encoder mode send rising edges and node1 calculates the speed and then this speed is sent via ttcan and node2 responses according to the state.
but still, the encoder has some jumps when press the keys, but it is pretty normal. probally it needs kalman filter to remove the unexpected jumps.


13.7
ttcan encoder number counting values in basic nodes(14) are different with warship bard(204), probably the timer configures wronly.
checking the difference, ttcan node will be around 18 times slower than the calculation in warship. check it after work

16.7 
warship board uses pwm control motor rotates and encoder values are read by node2 and then the node1 get the car speed states and call pwm to control the motor rotates speed again. here used T method,
while M method can also be executed here still the speed details still need to be specificated(v2.3)