1.想法，
串口输入的地方是不是相当于配置的PA9 的Tx？ 那么两个板子互相交叉接着，只要在1 板子上面不断的senddata，那就相当
在发送串口消息，那么此时2中断产生，不停保留数据，如果出现0x0d 和0x0a后， 其满足了条件： 0x8000，那么进入main函数
2将这个数据发送出去

USART_RX_STA if bit15 is 1 then it is 0x8000((2^3), if bit14 is 1 then it is 0x4000(2^2) 
0x8000((2^3): receiving finish flag
0x4000 means get 0x0d flag
bit13- 0 means the numbers of valid data
			
			
			
			
			
			
			
			
			
			
			
			
			
			