#include "can_config.h"
#include "usart.h"
#include "ttcan.h"
/*Mater Node*/
uint8_t MasterNumBC =0;
uint64_t MasterTimeTx =0;
/*Slave Node*/
uint8_t CAN_DATA0,CAN_DATA1,CAN_DATA2,CAN_DATA3,CAN_DATA4,CAN_DATA5,CAN_DATA6,CAN_DATA7;
void CanInit(void);
void CanFilter_0_Configure(void);
void 	CanFilter_1_Configure(void);
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
  CanInit();
  CanFilter_0_Configure();
  CanFilter_1_Configure();
  NVIC0_Configuration();
  NVIC1_Configuration();
  GPIO_Configuration(); // configure the can IO
}


void CanInit()
{
  CAN_InitTypeDef        CAN_InitStructure;  // CAN mode struct
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); // enable can1 clock.
  /* CAN register init */
  CAN_DeInit(CAN1);
  CAN_StructInit(&CAN_InitStructure);

  /* CAN cell init */
  CAN_InitStructure.CAN_TTCM = ENABLE; /* ʱ�䴥����ֹ, ʱ�䴥����CANӲ�����ڲ���ʱ����������ұ����ڲ���ʱ��� */
  CAN_InitStructure.CAN_ABOM = ENABLE; /* �Զ����߽�ֹ���Զ����ߣ�һ��Ӳ����ص�128��11������λ�����Զ��˳�����״̬��������Ҫ�����趨������˳� */
  CAN_InitStructure.CAN_AWUM = ENABLE;  /* �Զ����ѽ�ֹ���б�������ʱ���Զ��˳�����	*/
  CAN_InitStructure.CAN_NART = DISABLE; /* �����ش�, �������һֱ�����ɹ�ֹ������ֻ��һ�� */
  CAN_InitStructure.CAN_RFLM = DISABLE; /* receive FIFO lock, 1-- after locking, newly coming data will be ignored.0--���յ��µı����򸲸�ǰһ����	*/
  CAN_InitStructure.CAN_TXFP = DISABLE;  /* FIFO priority, 0 means decide by identifier, 1 means decided by request sequence	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack; /*better for testing. when 2 boards communication, then it is mode_normal 	*/
  CAN_InitStructure.CAN_Mode = CAN_Mode_Normal; /* ģʽ	*/
  //CAN_InitStructure.CAN_Mode = CAN_Mode_Silent ;
  CAN_InitStructure.CAN_SJW = CAN_SJW_4tq;      /* ����ͬ��������ֻ��canӲ�����ڳ�ʼ��ģʽʱ���ܷ�������Ĵ��� */
  CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;      /* PTS + PBS1 */
  CAN_InitStructure.CAN_BS2 = CAN_BS2_1tq;      /* PBS2 */
  CAN_InitStructure.CAN_Prescaler = 45;         /* ������Ԥ��Ƶ�� */
  CAN_Init(CAN1,&CAN_InitStructure);
  if (CAN_Init(CAN1,&CAN_InitStructure) == CANINITFAILED)
    {
    }
}

void CanFilter_0_Configure()
{ 
//	CAN_FilterInitStructure.CAN_FilterIdHigh   =(((u32)slave_id<<3)&0xFFFF0000)>>16;
//  CAN_FilterInitStructure.CAN_FilterIdLo=(((u32)slave_id<<3)|CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF;

  CAN_FilterInitTypeDef  CAN_FilterInitStructure0;  // CAN filter struct
  CAN_FilterInitStructure0.CAN_FilterActivation=ENABLE;
  CAN_FilterInitStructure0.CAN_FilterFIFOAssignment=0;  /* �ܹ�ͨ���ù������ı��Ĵ浽fifo0�� */
  CAN_FilterInitStructure0.CAN_FilterNumber=0;     /* ������0 */
  CAN_FilterInitStructure0.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure0.CAN_FilterScale=CAN_FilterScale_32bit; /* 32λ */
  //CAN_FilterInitStructure0.CAN_FilterIdHigh= ((((u32)MaskID_SM<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF0000)>>16;
	CAN_FilterInitStructure0.CAN_FilterIdHigh= ((((u32)MaskID_SM<<21))&0xFFFF0000)>>16; //MaskID_SM
  CAN_FilterInitStructure0.CAN_FilterIdLow= (((u32)MaskID_SM<<21)| CAN_ID_STD  |CAN_RTR_DATA)&0xFFFF;  //CAN_ID_EXT 
  CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE);   /* �Һ��ж�, �����жϺ��fifo�ı��ĺ����ͷű������жϱ�־ */
  CAN_FilterInitStructure0.CAN_FilterMaskIdHigh  = 0xFF9F;//MaskID_SM and MaskID_Ref both can reiceive
  CAN_FilterInitStructure0.CAN_FilterMaskIdLow   = 0xFFFF;
  CAN_FilterInit(&CAN_FilterInitStructure0);
}
void CanFilter_1_Configure()
{
  CAN_FilterInitTypeDef  CAN_FilterInitStructure1;  // CAN filter struct
  CAN_FilterInitStructure1.CAN_FilterActivation=ENABLE;
  CAN_FilterInitStructure1.CAN_FilterFIFOAssignment=1;  /* �ܹ�ͨ���ù������ı��Ĵ浽fifo0�� */
  CAN_FilterInitStructure1.CAN_FilterNumber=1;     /* ������0 */
  CAN_FilterInitStructure1.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure1.CAN_FilterScale=CAN_FilterScale_32bit; /* 32λ */
  CAN_FilterInitStructure1.CAN_FilterIdHigh= ((((u32)mes4_ID<<21))&0xFFFF0000)>>16;
  CAN_FilterInitStructure1.CAN_FilterIdLow= (((u32)mes4_ID<<21)| CAN_ID_STD  |CAN_RTR_DATA)&0xFFFF; //0x123
  CAN_ITConfig(CAN1,CAN_IT_FMP1, ENABLE);   /* �Һ��ж�, �����жϺ��fifo�ı��ĺ����ͷű������жϱ�־ */
  CAN_FilterInitStructure1.CAN_FilterMaskIdHigh  = 0xFF1F;
  CAN_FilterInitStructure1.CAN_FilterMaskIdLow   = 0xFFFF;
  CAN_FilterInit(&CAN_FilterInitStructure1);
}



void CanFilter_2_Configure()
{
  CAN_FilterInitTypeDef  CAN_FilterInitStructure1;  // CAN filter struct
  CAN_FilterInitStructure1.CAN_FilterActivation=ENABLE;
  CAN_FilterInitStructure1.CAN_FilterFIFOAssignment=1;  /* �ܹ�ͨ���ù������ı��Ĵ浽fifo0�� */
  CAN_FilterInitStructure1.CAN_FilterNumber=1;     /* ������0 */
  CAN_FilterInitStructure1.CAN_FilterMode=CAN_FilterMode_IdMask;  /* can be IDlist,IDmask, the mask is relatively complex*/
  CAN_FilterInitStructure1.CAN_FilterScale=CAN_FilterScale_32bit; /* 32λ */
  CAN_FilterInitStructure1.CAN_FilterIdHigh= ((((u32)MaskID_Ref<<3))&0xFFFF0000)>>16;
  CAN_FilterInitStructure1.CAN_FilterIdLow= (((u32)MaskID_Ref<<3)| CAN_ID_EXT  |CAN_RTR_DATA)&0xFFFF;
  CAN_ITConfig(CAN1,CAN_IT_FMP1, ENABLE);   /* �Һ��ж�, �����жϺ��fifo�ı��ĺ����ͷű������жϱ�־ */
  CAN_FilterInitStructure1.CAN_FilterMaskIdHigh  = 0xFFFF;
  CAN_FilterInitStructure1.CAN_FilterMaskIdLow   = 0xFFE7;
  CAN_FilterInit(&CAN_FilterInitStructure1);
}

void NodeMesTransmit(uint16_t ID)
{

  uint8_t box;
  CanTxMsg TxMessage;

  CAN_DATA0=0x22;
  CAN_DATA1=0x22;
  CAN_DATA2=0x22;
  CAN_DATA3=0x22;
  CAN_DATA4=0x5;
  CAN_DATA5=0x6;
  //CAN_DATA6=0x61;  CAN_DATA7=0x6e;

  /* transmit */
  TxMessage.StdId = ID;  /* ���ñ�׼id  ע���׼id�����7λ����ȫ������(1)����11λ */
  //TxMessage.ExtId = ID;     //������չid  ��չid��18λ
  TxMessage.RTR = CAN_RTR_DATA   ; /* data frame if it is remote frame, then the struct DATA[8] */
  TxMessage.IDE = CAN_ID_STD ; //CAN_ID_EXT   ;   /* ʹ�ñ�׼id	*/
  TxMessage.DLC = 8;            /* ���ݳ���, can���Ĺ涨�������ݳ���Ϊ8�ֽ� */

  TxMessage.Data[0] = CAN_DATA0;
  TxMessage.Data[1] = CAN_DATA1;
  TxMessage.Data[2] = CAN_DATA2;
  TxMessage.Data[3] = CAN_DATA3;
  TxMessage.Data[4] = CAN_DATA4;
  TxMessage.Data[5] = CAN_DATA5;
//  TxMessage.Data[6] = CAN_DATA6;
//  TxMessage.Data[7] = CAN_DATA7;
  box = CAN_Transmit(CAN1,&TxMessage);  /* ���������Ϣ�����͵������0,1,2��û���������뷢��no_box */
  delay_ms(100);
  /* transmit message wang adds */
  while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
  printf(">>>>>ID  %#x is sent<<<<<\r\n", ID);
}


void SendRef(uint16_t ID, uint16_t TOTNumBC, uint16_t NUMSlot, uint16_t SLOTTime)
{
  uint8_t box;
  CanTxMsg TxMessage;
  CAN_DATA0=TotNumBC;
  CAN_DATA1=NumSlot;
  CAN_DATA2=SlotTime;
  CAN_DATA3=MasterNumBC;
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
  MasterTimeTx += TxMessage.TimeStampTx;
  TxMessage.TimeStampTx =0;
  while(CAN_TransmitStatus(CAN1,box) == CANTXFAILED);
  printf("The sending reference time is  %lld \r\n", MasterTimeTx);
  //printf("\r\n the data has been sent already \r\n");

}