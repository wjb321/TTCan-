#include "ttcan.h"
#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"  
#include "can_config.h"

uint16_t NumOfSlot_flag = 0;	
uint16_t Slot_ISR_Timer = 4999;
uint64_t node_time = 0;
extern uint16_t mes_send_flag;
extern uint8_t SOS_ISR_Count; 
extern uint8_t CanFlag; 
extern uint64_t ref_time;
extern uint16_t CAN_ID;
extern uint8_t  TotNumBC_0, NumSlot_0, SLOTTime, NumBC, Rx_DATA3,Rx_DATA4,Rx_DATA5,Rx_DATA6,Rx_DATA7; 
extern uint8_t Rx_DATA0,Rx_DATA1,Rx_DATA2,Rx_DATA3,Rx_DATA4,Rx_DATA5,Rx_DATA6,Rx_DATA7;

/*master only*/
// origin matrix which is going to send 
uint16_t  mes_id[TotNumBC][NumSlot] = {{0x0001, 0x2623, 0x2620, 0x2624,0},
	                                    {0x0001, 0x2623, 0x2622, 0x2621, 0},
	                                    {0x0001, 0x2623, 0x2620, 0x2624, 0},
                                      {0x0001, 0x2623, 0x2622, 0x2625, 0}}; 

uint16_t is_exclusive_window[TotNumBC][NumSlot] = {{1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
	                                                {1, 1, 1, 1,0},
                                                  {1, 1, 1, 1,0}}; 

/*slave only */
// the intialized matrix which will store the sent matrix.
uint16_t  Received_mes_id[TotNumBC][NumSlot] ={0};
uint16_t received_is_exclusive_window[TotNumBC][NumSlot] = {0};
uint16_t selected_ID = 0;	
uint16_t mes_ids[4][4] = {0};	

//for master only
//uint8_t MatrixData0,MatrixData1,MatrixData2,MatrixData3,MatrixData4,MatrixData5,MatrixData6,MatrixData7;
//void SendMatrix()
//{
//	uint8_t box;
//  CanTxMsg MatrixTransmit;
////	MatrixData0=TotNumBC;  
////	MatrixData1=NumSlot;  
////  MatrixData2=SlotTime;  
////	MatrixData3=NumBC;  
////  MatrixData4=0x4;  
////	MatrixData5=0x5;
//  /* transmit */
//  //MatrixTransmit.StdId = Ref_ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
//  MatrixTransmit.ExtId = Master_ID;     //设置扩展id  扩展id共18位
//  MatrixTransmit.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
//  MatrixTransmit.IDE = CAN_ID_EXT  ;//CAN_ID_STD   ;   /* 使用标准id	*/
//  MatrixTransmit.DLC = 5;            /* 数据长度, can报文规定最大的数据长度为8字节 */
//	for(int i =0; i< TotNumBC; i++)
//	{
//		for(int j =0; j< NumSlot; j++)
//		 {
//	     MatrixTransmit.Data[0] = i;
//       MatrixTransmit.Data[1] = j;  			 
//       MatrixTransmit.Data[2] = mes_id[i][j];    
//       MatrixTransmit.Data[3] = mes_id[i][j]>>8;    
//       MatrixTransmit.Data[4] = is_exclusive_window[i][j];  
//       box = CAN_Transmit(CAN1,&MatrixTransmit); 
//	     while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
//			 printf("SM is sent2: %d\r\n", MatrixTransmit.Data[2]);
//			 printf("SM is sent3: %d \r\n", MatrixTransmit.Data[3]);
//			 printf("SM is sent4: %d \r\n", MatrixTransmit.Data[4]);
//     }
//	}
//	MatrixTransmit.Data[0] = TotNumBC * NumSlot;
//}
void SendMatrix(uint16_t ID)
{
	uint8_t box;
  CanTxMsg MatrixTransmit;
  /* transmit */
  //MatrixTransmit.StdId = Ref_ID;  /* 设置标准id  注意标准id的最高7位不能全是隐性(1)。共11位 */
  MatrixTransmit.ExtId = MaskID_SM;     //设置扩展id  扩展id共18位
  MatrixTransmit.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  MatrixTransmit.IDE = CAN_ID_EXT  ;//CAN_ID_STD   ;   /* 使用标准id	*/
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
			 delay_ms(300);
	     while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
	
			 printf("i = %d,j = %d  \r\n", i,j );
			 printf("SM is sent2: %#x\r\n", MatrixTransmit.Data[2]);
			 printf("SM is sent3: %#x \r\n", MatrixTransmit.Data[3]);
			 printf("SM is sent4: %#x \r\n", MatrixTransmit.Data[4]);
     }
	}
	MatrixTransmit.Data[0] = TotNumBC * NumSlot;
}


void node(void)
{ 

	if(CanFlag == ENABLE)
	  {
			CanFlag = DISABLE;
			printf("slave node 1 receives messages:\r\n");
			SOS_ISR_Count = 0;
			//printf("CAN ID %x \r\n",CAN_ID);
		  //printf("TotNumBC %x \r\n",TotNumBC);
			//printf("NumSlot %x \r\n",NumSlot);
			//printf("SlotTime %x \r\n",SlotTime);
			printf("NumBC %x \r\n",NumBC);
			//printf("MatrixData4 %x \r\n",Rx_DATA4);
			if(CAN_ID == MaskID_Ref)
			{
				//TIM3_Int_Init(Slot_ISR_Timer,7199);//10Khz的计数频率，计数到5000为500ms, 9999: 1 s 
		    //printf("Slot_ISR_Timer %x \r\n",Slot_ISR_Timer);				
			}
		}
		 if(mes_send_flag == ENABLE)
		{
			mes_send_flag = DISABLE;
			++SOS_ISR_Count; // start of slot isr times counter
			if(SOS_ISR_Count >= NumSlot){
				SOS_ISR_Count = 0;}
			printf("SOS_ISR_Count is %x \r\n",SOS_ISR_Count);
			selected_ID = mes_id[NumBC][SOS_ISR_Count];
			printf("selected_ID is %x \r\n",selected_ID);
			
		}
}




















