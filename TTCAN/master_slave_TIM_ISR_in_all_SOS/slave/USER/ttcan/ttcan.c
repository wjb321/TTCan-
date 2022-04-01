#include "ttcan.h"
#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"  
#include "can_config.h"


uint16_t NumOfSlot_flag = 0;	
uint16_t Slot_ISR_Timer = 4999;
uint64_t node_time = 0;
uint64_t timeTx =0;
uint8_t NumBC = 0;
uint16_t TimerArray[NumSlot-2] = {0,0,0}; // -2 is because RefCol and ArbCol wont take into consideration
extern uint16_t SOS_ISR_flag;
extern uint8_t SOS_ISR_Count; 
extern uint64_t ref_time;
extern uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
extern uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5,Rx1_DATA6,Rx1_DATA7;

/*slave only */
// the intialized matrix which will store the sent matrix.
uint16_t  Received_mes_id[TotNumBC][NumSlot] ={1};
uint8_t received_is_exclusive_window[TotNumBC][NumSlot] = {0};
uint16_t selected_ID = 0;	
uint16_t mes_ids[4][4] = {0};	


/*master only*/
// origin matrix which is going to send 
uint16_t  mes_id[TotNumBC][NumSlot] = {{0x0001, 0x2623, 0x2620, 0x2624,0},
	                                    {0x0001, 0x2623, 0x2622, 0x2621, 0},
	                                    {0x0001, 0x2623, 0x2620, 0x2624, 0},
                                      {0x0001, 0x2623, 0x2622, 0x2621, 0}}; 

uint8_t is_exclusive_window[TotNumBC][NumSlot] = {{1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
                                                  {1, 1, 1, 1,0}}; 

// message lists with the period and transmission time. 0, mesID; 1, period; 2, transmission time;
// the list can be the benchmark for the all the information, can store in all the nodes
uint16_t  Mes_list[NumMes][MesList] = {{0x2623, 10000, 5000},//
	                                       {0x2622, 20000,2000},
	                                       {0x2620, 20000, 15000},//
                                         {0x2624, 30000, 13000},//
																			   {0x2625, 40000, 1500},
																			   {0x2621, 50000, 1300}}; 

																			
void TimerSet()
{
	uint16_t temp_Mes_List =0;
	uint16_t temp_SM_ID =0;
	uint16_t TimerValue =0;
	uint8_t TimerArray_i = 0;
	for(int i =1; i<NumSlot; i++)
	{
		temp_SM_ID = Received_mes_id[0][i];
		for(int j =0; j<NumSlot; j++)
	  {
		  temp_Mes_List = Mes_list[j][0];
		if(temp_SM_ID==temp_Mes_List)
		 {
			TimerValue = Mes_list[j][2];
			 TimerArray[TimerArray_i] = TimerValue;
			 printf("timer array is: %d \r\n", TimerArray[TimerArray_i]);	
			 TimerArray_i++;
			 
		 }
		 
		}	
   	 
	}			

}


//for master only
//uint8_t MatrixData0,MatrixData1,MatrixData2,MatrixData3,MatrixData4,MatrixData5,MatrixData6,MatrixData7;

void SendMatrix(uint16_t ID)
{
	uint8_t box;
  CanTxMsg MatrixTransmit;
  /* transmit */
  MatrixTransmit.StdId = MaskID_SM;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  //MatrixTransmit.ExtId = Master_ID;     //设置扩展id  扩展id共18位
  MatrixTransmit.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  MatrixTransmit.IDE = CAN_ID_STD   ;   /* 使用标准id	*/
  MatrixTransmit.DLC = 5;            /* 数据长度, can报文规定最大的数据长度为8字节 */
	for(int i =0; i< TotNumBC; i++)
	{
		for(int j =0; j< NumSlot; j++)
		 {
	     MatrixTransmit.Data[0] = i;
       MatrixTransmit.Data[1] = j;  			 
       MatrixTransmit.Data[2] = mes_id[i][j];    
       MatrixTransmit.Data[3] = mes_id[i][j]>>8;    
       MatrixTransmit.Data[4] = is_exclusive_window[i][j];  
       box = CAN_Transmit(CAN1,&MatrixTransmit); 
	     while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
     }
	}
	MatrixTransmit.Data[0] = TotNumBC * NumSlot;
}


int i,j = 0;
void ReiceiveSM()
{
	   i = Rx0_DATA0;
		 j = Rx0_DATA1;
		 Received_mes_id[i][j] = Rx0_DATA2;
		 Received_mes_id[i][j] |= (uint16_t)Rx0_DATA3 << 8;
		 received_is_exclusive_window[i][j] = Rx0_DATA4;
	   if (i* j >=0)  // TotNumBC *NumSlot 
	     {
				 for(int i =0; i< TotNumBC; i++)
				 {
					for(int j =0; j< NumSlot; j++)
					{
					 	printf("received matrix  %d %d %#x \r\n", i, j, Received_mes_id[i][j]);
					 }
				 }
		   }
}





















