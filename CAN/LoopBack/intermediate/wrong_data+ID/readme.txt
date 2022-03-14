
Problems:
do the loopback test, there are two different nodes A, B.
1) try these nodes with this code, but A generates the data is what I want, B has different data in CAN_DATA6 and 
CAN_DATA7 parts, when there are CAN_DATA0-7

2) 2 nodes they both have changed ID values. 
    for example: I set ID = 0x22,  while the serial ports show ID is 0xXX22, where XX means random values from 0-e in hex.
    
    
Solved:
1) set ID, CAN_ID is set in can_config file in int8_t format, other files(.it, main) use as extern int16_t format, so the first XX in XX22 will be set randomly.
so solve this problem should be keeping same data format in different files.

2）as for data6 and data7, cause i enable ttcan, which uses the data6 and data7 as the counter, that's why it generates random values. while there is one node can 'safely' give the expected data, actually it is abnormal.  
    
