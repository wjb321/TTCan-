
Problems:

do the loopback test, there are two different nodes A, B.
  1) try these nodes with this code, A generates the data is what I want while B has different data in CAN_DATA6 and
    CAN_DATA7 parts, when there are CAN_DATA0-7

    ���ڽڵ�����⣺

    1�� ���ڵ���ܵ�refСʱʱ�򣬾ͻ�BC ++;
    ��Ϊ��Ϊ�������ڵı�־�����Ƕ��ھ�����Ϣ�ķ���Ӧ�ÿ����Ǵ�BC=0 ��ʼ���еġ�
        ����Ӧ�ý���BC-1��������BC0�Ĳ���������Ϊ�˷�ֹBC=0 ʱ�򣬻����BC-	1 = -1�����.�ͽ��������0.

            2)  ��ʱ�������ˣ�ʹ��������ʱ����T3 ��T2��
    T3�� ���ڲ������ڣ�����ÿ���ȶ��Ĵ��ڿ�ʼ�������жϲ���ʼ���� ���ݽ��յ���ϵͳ���󣬵õ���ͬ�Ĵ��ڴ�С����Ϊ��ͬ����Ϣ����ʱ�䲻ͬ����
    T2�� ������Ϣ�ķ����е�transmission time �����͵�ʱ�䣬��ô��T2�����ж�Ӧϵͳ�����ID���и���ʱ��ĳ������͡�����T3�����ж�ʱ�򣬶�T3 ��T2 ͬʱ���г�ʼ����
    T2 ��ʼ��ʱ����Ƕ�Ӧ�б��е�ID�ķ��ͣ����ж�ʱ�򣬶�Ӧ��ID��ֹͣ���͡����Ǿ��������ǲ��Եġ���Ϊÿ����Ӧ�Ĵ���ֻ��Ҫ����һ�θ�����window�µ�IDֵ��transmission
    time ���������Ϣ�ڷ��ͺ���CAN�����ϵĳ���ʱ�䡣����֮����ҪT���Ĳ��룬��Ϊ������ʱ�����з��͵���Ϣ��������ġ�T�����Դ���ĳ����Ϣ�ġ�����ʱ�䡱�������Բ���ȫռ��
    ������window����Ȼ���ռ������window�������������һ�����⣺���T�����T���ڸ�����window��̫�̣���ôT���п��ܷ������μ��������ϵ��жϣ���ô���ܻὫ��Ӧ��ID���Ͷ�Σ�
    �Ⲣ�������Ӧ��Ϣ���������ԡ�

    ����Ӧ�������ַ�����

    T����ʱ�����ڵõ�����֮�󣬾Ϳ�ʼ��ʼ����ʱ������ʱ����ʱ����ݲ�ͬ�е�slot���Ƚ��е���������ÿ�Σɣӣҵ�ʱ�����һ�γ�ʼ������ô����֮��ͨ�����ַ�ʽ�͵õ���Ӧ��exclusive��window��
    ��ô�ڶ�Ӧ�ĳ�ʼ��ʱ�򣬾Ͷ���Ӧ����Ϣ���з��͡�Ȼ��T���жϵ�������ʱ�䴰�ڽ�ֹ���������µ�T�����ڳ�ʼ������ʱ�ֽ��ж�Ӧ���ڵ�ID�ķ��ͣ�ֱ��������Ϣ������ϡ�



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

v1.1 ttcan.c file has configurations for slaveNode1 and slaveNode2, when do the updating and delete the brace by mistake and added in a wrong place. find the bug for a long time.
but it is not convinent when change to different node, i have to come to ttcan.c to configure again. so better way is using a interface then can change to any node.
v1.2 now: change node in main switch part only.

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