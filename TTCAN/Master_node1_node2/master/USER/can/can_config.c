#include "can_config.h"
#include "ttcan.h"
#include "usart.h"

uint16_t CAN_ID;
uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
uint8_t CanFlag;
uint64_t timeTx =0;
uint16_t NumBC =0;
extern uint16_t  mes_id[TotNumBC][NumSlot];

/*******************************************************************************
* Function Name  : CAN_mode_Configuration
* Description    : contains CAN mode and filter congiguration
* Input          : None
* Output         : None
* Return         : None
* Attention		   : 1) for testing, better use loopback mode, after testing then use normal mode
*******************************************************************************/

void CAN_Configuration(void)
{
  CAN_InitTypeDef        CAN_InitStructure;  // CAN mode struct
  CAN_FilterInitTypeDef  CAN_FilterInitStructure;  // CAN filter struct
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // enable can1 clock.
  NVIC_Configuration();
  GPIO_Configuration(); // configure the can IO
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = ENABLE; /* ʱ�䴥����ֹ, ʱ�䴥����CANӲ�����ڲ���ʱ����������ұ����ڲ���ʱ��� */
  CAN_InitStructure.CAN_ABOM = ENABLE; /* �Զ����߽�ֹ���Զ����ߣ�һ��Ӳ����ص�128��11������λ�����Զ��˳�����״̬��������Ҫ����趨������˳� */
  CAN_InitStructure.CAN_AWUM = ENABLE;  /* �Զ����ѽ�ֹ���б�������ʱ���Զ��˳�����	*/
  CAN_InitStructure.CAN_NART = DISABLE; /* �����ش�, �������һֱ�����ɹ�ֹ������ֻ��һ�� */
  CAN_InitStructure.CAN_RFLM = DISABLE; /* receive FIFO lock, 1-- after locking, newly coming data will be ignored.0--���յ��µı����򸲸�ǰһ����	*/
  CAN_InitStructure.CAN_TXFP = DISABLE;  /* FIFO priority, 0 means decide by identifier, 1 means decided by request sequence	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; /*better for testing. when 2 boards communication, then it is mode_normal 	*/
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* ģʽ	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_Silent ;
  CAN_InitStructure.CAN_SJW = CAN_SJW_4tq;      /* ����ͬ������ֻ��canӲ�����ڳ�ʼ��ģʽʱ���ܷ�������Ĵ��� */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;      /* PTS + PBS1 */
  CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;      /* PBS2 */
  CAN_InitStructure.CAN_Prescaler = 45;         /* ������Ԥ��Ƶ�� */
  /* �����ʼ��㷽�� */
  /* CANbps= Fpclk/((BRP+1)*((Tseg1+1)+(Tseg2+1)+1)  �˴�����Ϊ  CANbps=36000000/(45*(4+3+1))=100kHz */   														  //�˴�Tseg1+1 = CAN_BS1_8tp
  /* ���ô���: Tseg1>=Tseg2  Tseg2>=tq; Tseg2>=2TSJW */
  // wang configure
  CAN_Init(CAN1,&CAN_InitStructure);
  if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)
    {
    }
		
	//13.04,22�� �����������ȼ���ο����Ĳο��ֲ��433ҳ
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;  /* �ܹ�ͨ���ù������ı��Ĵ浽fifo0�� */
  CAN_FilterInitStructure.CAN_FilterNumber=0;     /* ������0 */
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; /* 32λ */
  //	CAN_FilterInitStructure.CAN_FilterIdHigh= ((((u32)0x2616<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;
  //	CAN_FilterInitStructure.CAN_FilterIdLow= (((u32)0x2616<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<3 |CAN_ID_STD|CAN_RTR_DATA )&0xffff0000)>>16;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<21)&0xffff0000);
  //  CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)mask_id<<3)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
  //  CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0xFFFF;
  //  CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0xFFFF;
  CAN_FilterInitStructure.CAN_FilterIdHigh= 0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow= 0x0000;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<3 |CAN_ID_STD|CAN_RTR_DATA )&0xffff0000)>>16;
  //	CAN_FilterInitStructure.CAN_FilterIdHigh = (((u32)mask_id<<21)&0xffff0000);
  //  CAN_FilterInitStructure.CAN_FilterIdLow = (((u32)mask_id<<3)|CAN_ID_STD|CAN_RTR_DATA)&0xffff;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh  = 0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow   = 0x0000;
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* �Һ��ж�, �����жϺ��fifo�ı��ĺ����ͷű������жϱ�־ */
  CAN_FilterInit(&CAN_FilterInitStructure);
}

//void CanWriteData(uint16_t ID, uint16_t TotNumBC, uint16_t NumSlot, uint16_t SlotTime);//uint16_t NumBC
void SendRef(uint16_t ID, uint16_t TOTNumBC, uint16_t NUMSlot, uint16_t SLOTTime)
{
  uint8_t box;
  CanTxMsg TxMessage;
  CAN_DATA0=TotNumBC;
  CAN_DATA1=NumSlot;
  CAN_DATA2=SlotTime;
  CAN_DATA3=NumBC;
  CAN_DATA4=0x4;
  CAN_DATA5=0x5;

  /* transmit */
  TxMessage.StdId = ID;  /* ���ñ�׼id  ע���׼id�����7λ����ȫ������(1)����11λ */
 // TxMessage.ExtId = ID;     //������չid  ��չid��18λ
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_STD; //CAN_ID_EXT   ;   /* ʹ�ñ�׼id	*/
  TxMessage.DLC = 8;            /* ���ݳ���, can���Ĺ涨�������ݳ���Ϊ8�ֽ� */

  TxMessage.Data[0] = CAN_DATA0;
  TxMessage.Data[1] = CAN_DATA1;
  TxMessage.Data[2] = CAN_DATA2;
  TxMessage.Data[3] = CAN_DATA3;
  TxMessage.Data[4] = CAN_DATA4;
  TxMessage.Data[5] = CAN_DATA5;
  box = CAN_Transmit(CAN1,&TxMessage);  /* ���������Ϣ�����͵������0,1,2��û���������뷢��no_box */

  /* transmit message wang adds */
  timeTx += TxMessage.TimeStampTx;
  TxMessage.TimeStampTx =0;
  while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
  printf("The sending reference time is  %lld \r\n", timeTx);
  //printf("\r\n the data has been sent already \r\n");

}
