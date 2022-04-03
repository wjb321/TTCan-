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
uint16_t Node1MesIDList[6] = {mes1_ID, mes2_ID, mes3_ID, mes4_ID, mes5_ID, mes6_ID};
uint16_t Node2MesIDList[3] = {mes4_ID, mes5_ID, mes6_ID};

extern int TimerArray_p;
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
uint16_t  mes_id[TotNumBC][NumSlot] = {{0x0001, 0x2623, 0x2620,0x2624,0},
	                                    {0x0001, 0x2623, 0x2622, 0x2621, 0},
	                                    {0x0001, 0x2623, 0x2620, 0x2624, 0},
                                      {0x0001, 0x2623, 0x2622, 0x2621, 0}}; 

uint8_t is_exclusive_window[TotNumBC][NumSlot] = {{1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
                                                  {1, 1, 1, 1,0}}; 

// message lists with the period and transmission time. 0, mesID; 1, period; 2, transmission time;
// the list can be the benchmark for the all the information, can store in all the nodes
uint16_t  Mes_list[NumMes][MesList] = {{0x2623, 10000, 7000},//
	                                       {0x2622, 20000,2000},
	                                       {0x2620, 20000, 3000},//
                                         {0x2624, 30000, 4000},//
																			   {0x2625, 40000, 1500},
																			   {0x2621, 50000, 1300}}; 

																				 
uint16_t SOS_ID()
{
	      uint8_t temp_MesTranTime = 0;
			  TimerArray_p ++;
				TIM3_Int_Init(TimerArray[TimerArray_p],7199,1); // this gets from timerset()
	printf("timer location: %d  in  sos isr with time: %d \r\n", TimerArray_p, TimerArray[TimerArray_p]);	
			  SOS_ISR_Count++;
	     if(SOS_ISR_Count > NumSlot-3)  //5-2 = 3
				 {
				   TIM_Cmd(TIM3, DISABLE);
				   TIM_Cmd(TIM2, DISABLE);
   			 }
			  if(NumBC == 0){
				NumBC = 1; }
			//将来就在此处通过对应的Reiceived mes 中的值进行设置来进行判断从而进行其是属于哪一个node,然后进行操作。
			  printf("send ID:[NumBC, SOS_ISR_Count] = [%d , %d] \r\n", NumBC-1, SOS_ISR_Count+1);
		  return  Received_mes_id[NumBC-1][SOS_ISR_Count+1];
}


extern uint8_t Transmit_time_flag;
int MesTransmitTime(uint16_t ID)
{
	int Mes_Tran_time =0;
	for(int i = 0; i <= NumMes; i++)
	{
		if(Mes_list[i][0] == ID)
		{
			Mes_Tran_time = Mes_list[i][2];
		}
	}
	
	return Mes_Tran_time;
}
void MesTransmit()
{
	for(int i = 0; i <= sizeof(Node1MesIDList); i++)
	{
		if(Node1MesIDList[i] == Received_mes_id[NumBC-1][SOS_ISR_Count+1])
		{ 
			printf("the value in the ID list is %#x \r\n",Node1MesIDList[i]);
			NodeMesTransmit(Node1MesIDList[i]);
		}
	}
}



/*
function:
checking the SM first row(BC0) IDs in PSA list to get transmission time of resepctively IDs
*/
void TimerSlotSet()
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
























