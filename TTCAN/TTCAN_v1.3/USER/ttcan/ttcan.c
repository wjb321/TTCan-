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
uint16_t Node1MesIDList[3] = {mes1_ID, mes2_ID, mes3_ID};
uint16_t Node2MesIDList[3] = {mes4_ID, mes5_ID, mes6_ID};
//uint16_t Node2MesIDList[3] = {mes4_ID, mes5_ID, mes6_ID};
extern uint16_t SOS_ISR_flag;
extern uint16_t interrupt_sos_times;
extern int MesTimesInBC;
extern uint64_t ref_time;
extern uint8_t Rx0_DATA0,Rx0_DATA1,Rx0_DATA2,Rx0_DATA3,Rx0_DATA4,Rx0_DATA5,Rx0_DATA6,Rx0_DATA7;
extern uint8_t Rx1_DATA0,Rx1_DATA1,Rx1_DATA2,Rx1_DATA3,Rx1_DATA4,Rx1_DATA5,Rx1_DATA6,Rx1_DATA7;

/*slave only */
// the intialized matrix which will store the sent matrix.
uint16_t  Received_mes_id[TotNumBC][NumSlot] = {1};
uint8_t received_is_exclusive_window[TotNumBC][NumSlot] = {0};
uint16_t finalTimerValue[NumSlot] = {0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
uint16_t selected_ID = 0;
uint16_t mes_ids[4][4] = {0};
int TimerArrayLocation = 0; //������λ�ô�receivedID��ת����timerArray �ϵ�λ��
int IDsInOneBC[TotNumBC] = {0};
extern uint8_t   SlaveNode1Flag;
extern uint8_t   SlaveNode2Flag;

/*master only*/
// origin matrix which is going to send
//uint16_t  mes_id[TotNumBC][NumSlot] = {{0x0001, 0x2623, 0x2620,0x2624,0},
//  {0x0001, 0x2623, 0x2622, 0x2621, 0},
//  {0x0001, 0x2623, 0x2620, 0x2624, 0},
//  {0x0001, 0x2623, 0x2622, 0x2625, 0}
//};

uint16_t  mes_id[TotNumBC][NumSlot] = {{0x001, 0x122, 0x123, 0x120,0},
  {0x001, 0x123, 0x121, 0x123, 0},
  {0x001, 0x122, 0x123, 0x121, 0},
  {0x001, 0x121, 0x122, 0x124, 0}
};
//uint16_t  mes_id[TotNumBC][NumSlot] = {{0x0001, 0x2624, 0x2621, 0x2624,0},
//  {0x0001, 0x2621, 0x2621, 0x2622, 0},
//  {0x0001, 0x2624, 0x2623, 0x2623, 0},
//  {0x0001, 0x2621, 0x2624, 0x2622, 0}
//};
uint8_t is_exclusive_window[TotNumBC][NumSlot] = {{1, 1, 1, 1,0},
  {1, 1, 1, 1,0},
  {1, 1, 1, 1,0},
  {1, 1, 1, 1,0}
};

// message lists with the period and transmission time. 0, mesID; 1, period; 2, transmission time;
// the list can be the benchmark for the all the information, can store in all the nodes
uint16_t  Mes_list[NumMes][MesList] = {{0x123, 10000, 7000},//
  {0x122, 20000, 2000},
  {0x120, 20000, 3000},//
  {0x124, 30000, 4000},//
  {0x125, 40000, 1500},
  {0x121, 50000, 2300}
};

int finalTimerArrayShift_Nonzero = 1; // ��־����ʱ��������û�д�0 ��ʼ�����[200, 300, XXX, ...]
int finalTimerArrayShift_zero = 1; // ��־����ʱ��������û�д�0 ��ʼ�����[0, 300, XXX,...]
int IDsInOneBC_shift_Nonzero = 0;
int IDsInOneBC_shift_zero = 1;
uint16_t SOS_ID()
{
  if(finalTimerValue[0]== 0)
    {
      printf("!!!!!!! finalTimerValue[0]== 0: NumBC: %d first message is sent:!!!!!!!\r\n", NumBC-1);
      NodeMesTransmit(Received_mes_id[NumBC-1][IDsInOneBC[finalTimerArrayShift_zero]]);
      //printf("this message %#x is sent. \r\n",Received_mes_id[NumBC-1][IDsInOneBC[finalTimerArrayShift_zero]]);
      //printf("IDsInOneBC[finalTimerArrayShift_zero] is %d\r\n", IDsInOneBC[finalTimerArrayShift_zero]);
      //printf("the messages times are %d in BC %d\r\n", MesTimesInBC, NumBC-1);
      //printf("!!!!!!! finalTimerValue[0]== 0 interrupt_sos_times: %d for message finish !!!!!!! \r\n", interrupt_sos_times);

      finalTimerArrayShift_zero++; //��++����Ϊ1��ʱ����CanRefFlag == ENABLE��λ0��ʱ���Ѿ�����
      //printf("finalTimerArrayShift_zero is %d\r\n", finalTimerArrayShift_zero);
      if(interrupt_sos_times +1  >= MesTimesInBC)  //5-2 = 3
        {
          TIM_Cmd(TIM3, DISABLE);
        }
      else
        {
          TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_zero],7199,ENABLE); // this gets from timerset()
        }

    }
  else
    {
      printf("!!!!!!! finalTimerValue[0]!= 0: NumBC-1: %d first message is sent:!!!!!!!\r\n", NumBC-1);
      NodeMesTransmit(Received_mes_id[NumBC-1][IDsInOneBC[finalTimerArrayShift_Nonzero-1]]);
     // printf("this message %#x is sent. \r\n",Received_mes_id[NumBC-1][IDsInOneBC[finalTimerArrayShift_Nonzero-1]]);
      //printf("IDsInOneBC[finalTimerArrayShift_Nonzero] is %d\r\n", IDsInOneBC[finalTimerArrayShift_Nonzero]);
     // printf("the messages times are %d in BC %d\r\n", MesTimesInBC, NumBC-1);
    //  printf("!!!!!!! finalTimerValue[0]!= 0 SOS_ISR_Count: %d for message finish !!!!!!! \r\n", interrupt_sos_times);
     // printf("finalTimerArrayShift_zero is %d\r\n", finalTimerArrayShift_Nonzero);
      if(interrupt_sos_times >= MesTimesInBC)  //5-2 = 3
        {
          TIM_Cmd(TIM3, DISABLE);
        }
      else
        {
          TIM3_Int_Init(finalTimerValue[finalTimerArrayShift_Nonzero],7199,ENABLE); // this gets from timerset()
          //printf("Now the finalTimerValue shift value is %d\r\n", finalTimerArrayShift_Nonzero);
          finalTimerArrayShift_Nonzero++; //��++��CanRefFlag == ENABLE ��û��ʹ��
        }

    }


  // SOS_ISR_Count++;

  //�������ڴ˴�ͨ����Ӧ��Reiceived mes �е�ֵ���������������жϴӶ���������������һ��node,Ȼ����в�����
  printf("send ID:[NumBC-1, interrupt_sos_times] = [%d , %d] \r\n", NumBC-1, interrupt_sos_times);
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
      if( SlaveNode1Flag == 1)
        {
          //printf("Node1MesIDList has IDs: =  %#x \r\n", Node1MesIDList[i]);
          if(Node1MesIDList[i] == Received_mes_id[NumBC][interrupt_sos_times])
            {

              //printf("the value in the ID list is i=%d  Node1MesIDList=  %#x \r\n",i, Node1MesIDList[i]);
              NodeMesTransmit(Node1MesIDList[i]);
              //printf("the value in the Received_mes_id[][] is %#x \r\n",Received_mes_id[NumBC][interrupt_sos_times]);
            }

        }

      else if (SlaveNode2Flag == 1)
        {
          if(Node2MesIDList[i] == Received_mes_id[NumBC][interrupt_sos_times])
            {

             // printf("the value in the ID list is i=%d  Node1MesIDList=  %#x \r\n",i, Node2MesIDList[i]);
              NodeMesTransmit(Node2MesIDList[i]);
             // printf("the value in the Received_mes_id[][] is %#x \r\n",Received_mes_id[NumBC][interrupt_sos_times]);
            }
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


uint16_t ArraySum(uint16_t *inputarray, uint16_t location)//��ͺ�������
{
  int sum = 0;//�ۻ�����ʼ��
  for (int i = 0; i < location; i++) //��ǰ��Ϣ��λ��֮ǰ����ֵ���
    {
      sum += *inputarray;
      inputarray++;
    }
  return sum;
}


int Locationjump = 0;
int tempMesLocation[TotNumBC*NumSlot][2];
int tempMesLocation_y = 0;
int tempTimerValue[NumSlot] = {0}; // ����ÿ����Ϣʱ���BC��ʼ��ֵ��exp [a,a+b, a+b+c+d,.....]
uint16_t everyBCmesTimes = 0;
int TimerISR()  //uint16_t *
{
  int tempMesLocation_x = 0;
  uint16_t tempMesID = 0;

  for(int j = 0; j < TotNumBC; j++)
    {
      for(int k = 0; k < NumSlot; k++)
        {
          tempMesID = Received_mes_id[j][k];
          for(int i =0; i<3; i++)
            {
              if( SlaveNode1Flag == 1)
                {
                  if(tempMesID == Node1MesIDList[i])
                    {
                      tempMesLocation[tempMesLocation_x][0]  = j;
                      tempMesLocation[tempMesLocation_x][1]  = k; // slotlocation
                      tempMesLocation_x++;
                    }

                }
              else if (SlaveNode2Flag == 1)
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
    }
  //printf("###tempMesLocation_x###%d\r\n", tempMesLocation_x);
		
//  for(int m =0; m<tempMesLocation_x; m++)
//    {
//      printf("###jiabin wang###\r\n");
//      printf("relavant mes location: (%d, %d)\r\n", tempMesLocation[m][0],tempMesLocation[m][1] );
//      printf("@@@@@@@@@@@@@\r\n");
//    }

  int BCMestimes=0;
  //int IDsInOneBC[TotNumBC] = {0}; ������ȫ�ֱ�������Ϊÿ�ε��ô˺���֮�󣬶����0���и�д����֮ǰ��ֵ������
  // Locationjump��ֵ���ܳ���tempMesLocation_x��ֵ�����Ե�����ʱ��ͽ������㣬��b��ʹ��
  if(Locationjump >= tempMesLocation_x )
    {
      Locationjump = 0;
    }
// printf("tempMesLocation_x value is :%d\r\n", tempMesLocation_x );
// printf("Locationjump value is :%d\r\n", Locationjump );
  //b����a��ͬ�Ĺ��ܣ�ֻ�ǽ��Ѿ����к���в��ٽ����ٴ����У���ΪLocationjump��һֱ���
  for( int c =Locationjump; c<tempMesLocation_x; c++) //����ѡ��ȥ��Locationjump���ñ���һֱ��0���� tempInitMesLocation

    {

      // printf("tempMesLocation_x��is����%d \r\n",tempMesLocation_x);
      if(tempMesLocation[c][0] == NumBC)
        {
          IDsInOneBC[BCMestimes] = tempMesLocation[c][1] ;
         // printf("IDsInOneBC is  %d \r\n",IDsInOneBC[BCMestimes]);
          BCMestimes ++;//�õ��ڱ�BC�¶�ӦNode�ɷ���Ϣ�ĸ���
          Locationjump++; //�Ѿ���tempMesLocation����֤���ģ��´ξͲ���Ҫ�ٿ��ǣ���������ȥ���������ֵ��ʼ������global����ÿ��ִ�к���ʱ����Ҫ��0��
          //printf("BCMestimes is %d and Locationjump is %d \r\n",BCMestimes, Locationjump);
        }

    }

  uint16_t   tempTimerStore[NumSlot] = {0}, ValueStoreCounter = 0;

  for(int q =0; q< BCMestimes; q++ )
    {
      //printf("-----------\r\n");
      TimerArrayLocation =IDsInOneBC[q]-1 ;//-1;
      //printf(" in BC %d has messages from node1 is %d \r\n",NumBC, IDsInOneBC[q]);
      //printf(" the message time in the location of timerarray: %d \r\n",TimerArrayLocation);

      //tempTimerValue[q] = ArraySum(TimerArray, TimerArrayLocation);	//0, 2 ---> q =0,1
      tempTimerStore[ValueStoreCounter] = TimerArrayLocation;
      ValueStoreCounter++;
      // printf("  tempTimerValue[1]: %d \r\n", ArraySum(TimerArray,1));
      // printf("  tempTimerValue[2]: %d \r\n", ArraySum(TimerArray,2));
      // printf("  tempTimerValue[3]: %d \r\n", ArraySum(TimerArray,3));
      // printf(" 1: tempTimerValue[q]: %d \r\n", ArraySum(TimerArray,TimerArrayLocation));
      // printf(" 2:(q,tempTimerValue[q])= (%d, %d) \r\n",q, tempTimerValue[q]);
      //printf("-----------\r\n");
    }
  tempTimerValue[0] = ArraySum(TimerArray,tempTimerStore[0]);
  // �˴�������Ϣ����ISR֮���ʱ����ƣ�����һ��ʱ���ȥ��һ��ʱ�伴�ǲ����жϺ���Ҫ���õ���һ����Ϣ������ʱ��ֵ��
  for(int p =1; p< BCMestimes; p++ ) //ʼ�ղ����ǵ�һλ������p��1��ʼ
    {

      tempTimerValue[p] = ArraySum(TimerArray,tempTimerStore[p]);
      // printf("(0,tempTimerValue[0]) =(%d, %d) \r\n", 0, ArraySum(TimerArray,tempTimerStore[0]));
      // printf("(p,tempTimerValue[p]) = (%d ,%d )\r\n", p, tempTimerValue[p]);

    }

  for(int w =0; w< BCMestimes; w++ ) //ʼ�ղ����ǵ�һλ������p��1��ʼ
    {
      finalTimerValue[0] = tempTimerValue[0]; //����ʱ���ĵ�һ����Ϣʱ�����ʱʱ�����һ�µģ����Բ�����ѭ��
      if(w >= 1)
        {
          //printf(" 1111111111111111111\r\n");
          finalTimerValue[w] =   tempTimerValue[w] - tempTimerValue[w-1];
          //printf(" 1111111111111111111\r\n");
        }
      //printf("(w, templtimer) should be (%d, %d)\r\n",0, tempTimerValue[0]);
      // printf(" 1, finaltimer should be %d \r\n",finalTimerValue[1]);
     // printf("finaltimer should be %d \r\n",finalTimerValue[w]);
    }
  printf("tempMesLocation_x(messages times in one BC) should be %d \r\n",BCMestimes);
  return BCMestimes;

}







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
          delay_ms(600); // forget this delay function
          while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
          printf("i = %d,j = %d  \r\n", i,j );
          printf("SM is sent2: %#x\r\n", MatrixTransmit.Data[2]);
          printf("SM is sent3: %#x \r\n", MatrixTransmit.Data[3]);
          printf("SM is sent4: %#x \r\n", MatrixTransmit.Data[4]);
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
























