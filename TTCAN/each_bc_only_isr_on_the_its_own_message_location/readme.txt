
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
       
TTCAN： 发送节点配置为TTCAN模式后，那么其TDT_R的高16为会覆盖Data6,7的值，这个值就是对应的时间戳。
        接收端通过RDT_R 的高16位保存接收到的时间戳，此处经过更改stm3210fxCan.c/h 库文件，添加相应的时间戳参数给结构体同时加入时间戳值保存办法。参考对应文件的更改。
