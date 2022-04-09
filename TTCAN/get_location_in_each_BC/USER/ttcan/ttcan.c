#include "ttcan.h"
#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"  
#include "can_config.h"
//#include <numeric>

uint16_t NumOfSlot_flag = 0;	
uint16_t Slot_ISR_Timer = 4999;
uint64_t node_time = 0;
uint64_t timeTx =0;
uint8_t NumBC = 0;

uint16_t TimerArray[NumSlot-2] = {0,0,0}; // -2 is because RefCol and ArbCol wont take into consideration
uint16_t Node1MesIDList[3] = {0x2620, 0x2621, 0x2622};
uint16_t Node2MesIDList[3] = {0x2623, 0x2624, 0x2625};
//uint16_t Node2MesIDList[3] = {mes4_ID, mes5_ID, mes6_ID};

extern int TimerArray_p;
extern uint16_t SOS_ISR_flag;
extern uint16_t interrupt_sos_times, SOS_ISR_Count; 
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
                                      {0x0001, 0x2623, 0x2622, 0x2625, 0}}; 

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
																			   {0x2621, 50000, 2300}}; 

																				 
uint16_t SOS_ID()
{
	      uint8_t temp_MesTranTime = 0, timelist =0;
			  TimerArray_p ++;
	
	      //07/04.22
	      //timelist = TimerISR()[1];
			  printf("HHHH timelist: %d :++++++++++ \r\n", timelist);	
	
				TIM3_Int_Init(TimerArray[TimerArray_p],7199,1); // this gets from timerset()
	      printf("!!!!!!! SOS NumBC: %d first message is sent:!!!!!!!\r\n", NumBC);	
	      MesTransmit();
			  printf("!!!!!!! SOS SOS_ISR_Count: %d for message finish !!!!!!! \r\n", interrupt_sos_times);	
	      printf("timer: [location, value]: [%d,%d]\r\n", TimerArray_p, TimerArray[TimerArray_p]);	
			 // SOS_ISR_Count++;
	     if(interrupt_sos_times > NumSlot-3)  //5-2 = 3
				 {
				   TIM_Cmd(TIM3, DISABLE);
				   //TIM_Cmd(TIM2, DISABLE);
   			 }
//			  if(NumBC == 0){
//				NumBC = 1; }
			//�������ڴ˴�ͨ����Ӧ��Reiceived mes �е�ֵ���������������жϴӶ���������������һ��node,Ȼ����в�����
			  printf("send ID:[NumBC, interrupt_sos_times] = [%d , %d] \r\n", NumBC, interrupt_sos_times);
		  return  Received_mes_id[NumBC][interrupt_sos_times];
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
	for(int i = 0; i <3; i++)  //sizeof(Node1MesIDList)/sizeof(Node1MesIDList[0])
	{
		//printf("Node1MesIDList has IDs: =  %#x \r\n", Node1MesIDList[i]);
		if(Node1MesIDList[i] == Received_mes_id[NumBC][interrupt_sos_times])
		{ 
			
			printf("the value in the ID list is i=%d  Node1MesIDList=  %#x \r\n",i, Node1MesIDList[i]);
			NodeMesTransmit(Node1MesIDList[i]);
			printf("the value in the Received_mes_id[][] is %#x \r\n",Received_mes_id[NumBC][interrupt_sos_times]);
		}
		//else printf("nothing to send  \r\n");
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
		for(int j =0; j<NumMes; j++)
	  {
		  temp_Mes_List = Mes_list[j][0];
		  if(temp_SM_ID==temp_Mes_List)
		  {
			 TimerValue = Mes_list[j][2];
			 TimerArray[TimerArray_i] = TimerValue;
			 TimerArray_i++;
			}
		 }	
  }
 printf("timer list are: [%d, %d, %d] \r\n", TimerArray[0], TimerArray[1], TimerArray[2]);			
}


int TimerAccumulate(int Start_value, int End_value, uint16_t arr[])
{
	int TimerExactValue = 0;
	for(int i = 0; i< End_value - Start_value; i++ )
	{
		TimerExactValue += arr[i];
	}
	return TimerExactValue;
}



//int TimerISR()
//{ 
//	uint16_t tempMesID = 0;
//	uint16_t tempMesBC = 0;
//	uint16_t tempMesSlot = 0;
//	int tempMesLocation[5][2]= {{0,0}};
//	int tempMesLocation_x = 0,tempMesLocation_y = 0;
//	for(int i =0; i<3; i++)
//	{
//		tempMesID = Node1MesIDList[i];
//	  for(int j = 0; j < TotNumBC; j++)
//		{
//			 for(int k = 0; k < NumSlot; k++)
//			 {
//				 if(tempMesID == Received_mes_id[j][k])
//				 {
//					 tempMesLocation[tempMesLocation_x][0]  = j;
//					 tempMesLocation[tempMesLocation_x][1]  = k; // slotlocation
//		       tempMesLocation_x++;
//				 }
//			 }
//		}
//	}
//		for(int m =0; m<tempMesLocation_x; m++)
//	{   
//		 if(tempMesLocation[m][0] == 0)
//		 {
//			 
//		 }
//		  printf("@@@@@@@@@@@@@\r\n");	
//			printf("relavant mes location: (%d, %d)\r\n", tempMesLocation[m][0],tempMesLocation[m][1] );	
//      printf("@@@@@@@@@@@@@\r\n");	
//		  //TimerAccumulate(0,tempMesLocation[m][1],TimerArray);
//	}	

//return 	tempMesLocation[5][1];
//}

uint16_t ArraySum(uint16_t arrayname[], uint16_t location)//��ͺ�������
{
	int i;//ѭ������
	int sum = 0;//�ۻ�����ʼ��
	for (i = 0; i < location-1; i++) //��ǰ��Ϣ��λ��֮ǰ����ֵ���
	{
		sum += arrayname[i];
		arrayname++;
	}
	return sum;
}


int Locationjump = 0;
//int tempMesLocation[8][2];//= {{0,0}};
int tempMesLocation[TotNumBC*NumSlot ][2];
int tempMesLocation_y = 0;
  int tempTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
	static uint16_t finalTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
	uint16_t everyBCmesTimes = 0; 
	int TimerArrayLocation = 0; //������λ�ô�receivedID��ת����timerArray �ϵ�λ��
void TimerISR()  //uint16_t *
{
	int tempMesLocation_x = 0;
	uint16_t tempMesID = 0;
	//int tempMesBC = 0;
	//int tempMesSlot = 0, BCTimerCounter = 0;
	//int BCMestimes=0;
  //int tempTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
	//static uint16_t finalTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
	//int IDsInOneBC[TotNumBC] = {0};
	//uint16_t everyBCmesTimes = 0; 

  
	for(int j = 0; j < TotNumBC; j++)
		{
			 for(int k = 0; k < NumSlot; k++)
			 { 
				 tempMesID = Received_mes_id[j][k];
				 for(int i =0; i<3; i++)
				 {
					 
					if(tempMesID == Node1MesIDList[i])
				  {
					  tempMesLocation[tempMesLocation_x][0]  = j;
					  tempMesLocation[tempMesLocation_x][1]  = k; // slotlocation
		        tempMesLocation_x++;
				   }
				 }
	
			 }
		}
			printf("###tempMesLocation_x###%d\r\n", tempMesLocation_x);	
				for(int m =0; m<tempMesLocation_x; m++)
			{   
						printf("###jiabin wang###\r\n");	
						printf("relavant mes location: (%d, %d)\r\n", tempMesLocation[m][0],tempMesLocation[m][1] );	
						printf("@@@@@@@@@@@@@\r\n");	
//				if(tempMesLocation[m][0] == 2) //NumBC
//				 {
//					IDsInOneBC[BCMestimes] = tempMesLocation[m][1] ;
//					printf("GGGGGGG\r\n");	
//					printf("IDsInOneBC is  %d \r\n",IDsInOneBC[BCMestimes]);		
//					BCMestimes ++;//�õ��ڱ�BC�¶�ӦNode�ɷ���Ϣ�ĸ���
//					Locationjump++; //�Ѿ���tempMesLocation����֤���ģ��´ξͲ���Ҫ�ٿ��ǣ���������ȥ���������ֵ��ʼ������global����ÿ��ִ�к���ʱ����Ҫ��0��
//					printf("BCMestimes is %d and Locationjump is %d \r\n",BCMestimes, Locationjump);		
//					printf("GGGGGGG\r\n");	
//			   }
			}
			
			int BCMestimes=0;
	    int IDsInOneBC[TotNumBC] = {0};
        // Locationjump��ֵ���ܳ���tempMesLocation_x��ֵ�����Ե�����ʱ��ͽ������㣬��b��ʹ��
			if(Locationjump > sizeof(TotNumBC*NumSlot)+2)
			{
				Locationjump = 0;
			}	
			  //b����a��ͬ�Ĺ��ܣ�ֻ�ǽ��Ѿ����к���в��ٽ����ٴ����У���ΪLocationjump��һֱ���
			for( int c =0; c<tempMesLocation_x; c++) //����ѡ��ȥ��Locationjump���ñ���һֱ��0���� tempInitMesLocation
			
			{  

				// printf("tempMesLocation_x��is����%d \r\n",tempMesLocation_x);					
				if(tempMesLocation[c][0] == NumBC)
				 {
					IDsInOneBC[BCMestimes] = tempMesLocation[c][1] ;
					printf("IDsInOneBC is  %d \r\n",IDsInOneBC[BCMestimes]);	
          BCMestimes ++;//�õ��ڱ�BC�¶�ӦNode�ɷ���Ϣ�ĸ���							 
					Locationjump++; //�Ѿ���tempMesLocation����֤���ģ��´ξͲ���Ҫ�ٿ��ǣ���������ȥ���������ֵ��ʼ������global����ÿ��ִ�к���ʱ����Ҫ��0��
					printf("BCMestimes is %d and Locationjump is %d \r\n",BCMestimes, Locationjump);
          }
	
			}	
		
				for(int q =0; q< BCMestimes; q++ )
			{   
							printf("-----------\r\n");
				      TimerArrayLocation =IDsInOneBC[q] ;//-1;
						  //printf(" in BC %d has messages from node1 is %d \r\n",NumBC, IDsInOneBC[q]);
				      printf(" the message time in the location of timerarray: %d \r\n",TimerArrayLocation);
              tempTimerValue[q] = ArraySum(TimerArray, TimerArrayLocation);	
							printf("-----------\r\n");
			 }	
			 finalTimerValue[0] = tempTimerValue[0]; //����ʱ���ĵ�һ����Ϣʱ�����ʱʱ�����һ�µģ����Բ�����ѭ��
		// a)��ӡ���������ж�Ӧλ�õ���Ϣ
//		for(int m =0; m<tempMesLocation_x; m++)
//			{   
//						printf("###jiabin wang###\r\n");	
//						printf("relavant mes location: (%d, %d)\r\n", tempMesLocation[m][0],tempMesLocation[m][1] );	
//						printf("@@@@@@@@@@@@@\r\n");	
//				if(tempMesLocation[m][0] == 2) //NumBC
//				 {
//					IDsInOneBC[BCMestimes] = tempMesLocation[m][1] ;
//					printf("GGGGGGG\r\n");	
//					printf("IDsInOneBC is  %d \r\n",IDsInOneBC[BCMestimes]);		
//					BCMestimes ++;//�õ��ڱ�BC�¶�ӦNode�ɷ���Ϣ�ĸ���
//					Locationjump++; //�Ѿ���tempMesLocation����֤���ģ��´ξͲ���Ҫ�ٿ��ǣ���������ȥ���������ֵ��ʼ������global����ÿ��ִ�к���ʱ����Ҫ��0��
//					printf("BCMestimes is %d and Locationjump is %d \r\n",BCMestimes, Locationjump);		
//					printf("GGGGGGG\r\n");	
//			   }
//			}
//			// Locationjump��ֵ���ܳ���tempMesLocation_x��ֵ�����Ե�����ʱ��ͽ������㣬��b��ʹ��
//			if(Locationjump > sizeof(tempMesLocation_x)+1)
//			{
//				Locationjump = 0;
//			}	
//   
//			 int TimerArrayLocation = 0; //������λ�ô�receivedID��ת����timerArray �ϵ�λ��
//			 for(int q =0; q< BCMestimes; q++ )
//			{   
//							printf("-----------\r\n");
//				      TimerArrayLocation =IDsInOneBC[q] ;//-1;
//							printf(" in BC %d has messages from node1 is %d \r\n",NumBC, IDsInOneBC[q]);
//				      printf(" the message time in the location of timerarray: %d \r\n",TimerArrayLocation);
//              tempTimerValue[q] = ArraySum(TimerArray, TimerArrayLocation);	
//							printf("-----------\r\n");
//			 }	
//			 finalTimerValue[0] = tempTimerValue[0]; //����ʱ���ĵ�һ����Ϣʱ�����ʱʱ�����һ�µģ����Բ�����ѭ��
//			 
//			 
//			// �˴�������Ϣ����ISR֮���ʱ����ƣ�����һ��ʱ���ȥ��һ��ʱ�伴�ǲ����жϺ���Ҫ���õ���һ����Ϣ������ʱ��ֵ��
//			for(int p =1; p< BCMestimes; p++ ) //ʼ�ղ����ǵ�һλ������p��1��ʼ
//			{   
//        finalTimerValue[p] =   tempTimerValue[p] - tempTimerValue[p-1];
//			 } 
			
					
//			  for(int InitMesTimes = 0; InitMesTimes< BCMestimes; InitMesTimes++ ) //ÿ�����ڿ����м������ڽڵ�1����Ϣ������Ҫ����ÿ����Ϣ�������ʱ�䡣
//			 { 
//				 if (IDsInOneBC[InitMesTimes] == 1)
//				 { 
//					 
//				 }
//				 else if(IDsInOneBC[InitMesTimes] > 1)
//				 { 
//				   //int IDLeftShift = ;
//					 while(IDsInOneBC[0]--)
//					 {
//						 TimerValue[BCTimerCounter] += TimerArray[IDsInOneBC[0]-- -1];
//					 
//					 }
////					  for(int slotstimes = 0;slotstimes < IDsInOneBC[InitMesTimes] -1;slotstimes++) //��slot�µ�mesǰ����slotӦ�ñ�����ʱ�������С�
////    			 {
////					 TimerValue[BCTimerCounter] += TimerArray[IDsInOneBC[InitMesTimes]-slotstimes];
////				    }
//				 }
//				   		
//			}
      
//		 for(int w =0; w< BCMestimes; w++ ) //ʼ�ղ����ǵ�һλ������p��1��ʼ
//			 {   
//       printf(" finaltimer should be %d \r\n",finalTimerValue[w]);
//			 }
//return 	finalTimerValue;
}


//  int tempTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
//	static uint16_t finalTimerValue[] ={0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
//	uint16_t everyBCmesTimes = 0; 
//	int TimerArrayLocation = 0; //������λ�ô�receivedID��ת����timerArray �ϵ�λ��
//	
//void meslocation()
//{    
//     	int BCMestimes=0;
//	    int IDsInOneBC[TotNumBC] = {0};
//        // Locationjump��ֵ���ܳ���tempMesLocation_x��ֵ�����Ե�����ʱ��ͽ������㣬��b��ʹ��
//			if(Locationjump > sizeof(TotNumBC*NumSlot)+2)
//			{
//				Locationjump = 0;
//			}	
//			  //b����a��ͬ�Ĺ��ܣ�ֻ�ǽ��Ѿ����к���в��ٽ����ٴ����У���ΪLocationjump��һֱ���
//			for( int c =0; c<9; c++) //����ѡ��ȥ��Locationjump���ñ���һֱ��0���� tempInitMesLocation
//			
//			{  

//				// printf("tempMesLocation_x��is����%d \r\n",tempMesLocation_x);					
//				if(tempMesLocation[c][0] == NumBC)
//				 {
//					IDsInOneBC[BCMestimes] = tempMesLocation[c][1] ;
//					printf("IDsInOneBC is  %d \r\n",IDsInOneBC[BCMestimes]);	
//          BCMestimes ++;//�õ��ڱ�BC�¶�ӦNode�ɷ���Ϣ�ĸ���							 
//					Locationjump++; //�Ѿ���tempMesLocation����֤���ģ��´ξͲ���Ҫ�ٿ��ǣ���������ȥ���������ֵ��ʼ������global����ÿ��ִ�к���ʱ����Ҫ��0��
//					printf("BCMestimes is %d and Locationjump is %d \r\n",BCMestimes, Locationjump);
//          }
//	
//			}	
//		
//				for(int q =0; q< BCMestimes; q++ )
//			{   
//							printf("-----------\r\n");
//				      TimerArrayLocation =IDsInOneBC[q] ;//-1;
//						  //printf(" in BC %d has messages from node1 is %d \r\n",NumBC, IDsInOneBC[q]);
//				      printf(" the message time in the location of timerarray: %d \r\n",TimerArrayLocation);
//              tempTimerValue[q] = ArraySum(TimerArray, TimerArrayLocation);	
//							printf("-----------\r\n");
//			 }	
//			 finalTimerValue[0] = tempTimerValue[0]; //����ʱ���ĵ�һ����Ϣʱ�����ʱʱ�����һ�µģ����Բ�����ѭ��
//}



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
























