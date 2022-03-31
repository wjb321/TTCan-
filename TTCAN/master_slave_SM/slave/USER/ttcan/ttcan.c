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
extern uint16_t SOS_ISR_flag;
extern uint8_t SOS_ISR_Count; 
extern uint64_t ref_time;
extern uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
extern uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5,Rx1_DATA6,Rx1_DATA7;
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

/*slave only */
// the intialized matrix which will store the sent matrix.
uint16_t  Received_mes_id[TotNumBC][NumSlot] ={1};
uint8_t received_is_exclusive_window[TotNumBC][NumSlot] = {0};


uint16_t selected_ID = 0;	
uint16_t mes_ids[4][4] = {0};	
//for master only
//uint8_t MatrixData0,MatrixData1,MatrixData2,MatrixData3,MatrixData4,MatrixData5,MatrixData6,MatrixData7;
void SendMatrix(uint16_t ID)
{
	uint8_t box;
  CanTxMsg MatrixTransmit;
  /* transmit */
  MatrixTransmit.StdId = MaskID_SM;  /* ���ñ�׼id  ע���׼id�����7λ����ȫ������(1)����11λ */
  //MatrixTransmit.ExtId = Master_ID;     //������չid  ��չid��18λ
  MatrixTransmit.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  MatrixTransmit.IDE = CAN_ID_STD   ;   /* ʹ�ñ�׼id	*/
  MatrixTransmit.DLC = 5;            /* ���ݳ���, can���Ĺ涨�������ݳ���Ϊ8�ֽ� */
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





















