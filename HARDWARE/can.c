#include "can.h"

volatile uint8_t can1_rx_buff[CAN_BUFFER_SIZE];
volatile uint16_t can1_rx_buff_store = 0;
uint16_t can1_rx_buff_get = 0;

uint8_t CAN_Mode_Init(uint8_t mode)
{
	GPIO_InitTypeDef       GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);

//让PA11、PA12作为CAN的两根线使用	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//由于CAN默认高电平，信号起始位低电平，所以这里配置为高电平
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

//查表得知AF4是PA11、PA12复用到CAN	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_4);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_4);

	CAN_DeInit(CAN);
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;

	CAN_InitStructure.CAN_Mode= mode;//普通模式、环回模式、静默模式、环回静默模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler=8;
		
//设置完11个参数后，进行CAN初始化		
	CAN_Init(CAN, &CAN_InitStructure);// 初始化CAN1

//设置过滤器0，接收任何数据，只接受ID=1的数据
	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdList; 
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
	CAN_FilterInitStructure.CAN_FilterIdHigh=(DEVICE_TYPE_GAS<<5); //32位ID，这个保存ID[3:0]
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK，保存0000，代表广播
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

	CAN_ITConfig(CAN,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许
	NVIC_InitStructure.NVIC_IRQChannel = CEC_CAN_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	return 0;
}   
		   

void CEC_CAN_IRQHandler(void)
{
	uint8_t i;
	CanRxMsg RxMessage;
	
  CAN_Receive(CAN, CAN_FIFO0, &RxMessage);//CAN设备里的FIFO0的数据，读取出来存到RxMessage结构体里
	
	for (i = 0; i < RxMessage.DLC; i++)//按照数据长度往缓冲区里存储数据
	{
		can1_rx_buff[can1_rx_buff_store] = RxMessage.Data[i];
		
		can1_rx_buff_store++;
		if (can1_rx_buff_store == CAN_BUFFER_SIZE)
		{
			can1_rx_buff_store = 0;
		}
	}

	GPIO_ResetBits(GPIOA, GPIO_Pin_7);
	led_rx_count = 1;
}

//通过判断get和store的位置关系来决定是否有新值
uint8_t CAN1_rx_check(void)
{
	if (can1_rx_buff_store == can1_rx_buff_get)
		return 0;
	else 
		return 1;
}

//当有新值的时候，取出一个值
uint8_t CAN1_rx_byte(void)
{
	uint8_t ch;	
	
	ch = can1_rx_buff[can1_rx_buff_get];//用ch记录下来收到的一个数据
	
	can1_rx_buff_get++;
	if (can1_rx_buff_get == CAN_BUFFER_SIZE)
	{
		can1_rx_buff_get = 0;
	}
	
	return ch;
}

uint8_t Can_Send_Msg(uint8_t* msg,uint8_t len)
{	
  uint8_t mbox;
  uint16_t i=0;
  CanTxMsg TxMessage;						           
  TxMessage.StdId=(DEVICE_TYPE_GAS<<4);   
  TxMessage.ExtId=0x00;				             
  TxMessage.IDE=CAN_Id_Standard;          
  TxMessage.RTR=CAN_RTR_Data;		         
  TxMessage.DLC=len;						          
  for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];                
  
	mbox= CAN_Transmit(CAN, &TxMessage);    
	
  i=0;
  while((CAN_TransmitStatus(CAN, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	led_tx_count = 1;
	
  if(i>=0XFFF)
	{
		CAN_Mode_Init(CAN_Mode_Normal);
		return 1;
  }
	return 0;		//成功返回

}

//将子模块的数据返回给管理模块
uint8_t CAN1_send_message_fun(uint8_t *message, uint8_t len)
{
	//假设一共50个字
	uint8_t count;		             
	uint8_t time;
	
	time = len/8;           
	
	for (count = 0; count < time; count++)
	{
		Can_Send_Msg(message, 8);
		message += 8;
		delay_xms(1);
	}
	if (len%8)                         
	{
		Can_Send_Msg(message, len%8);
		delay_xms(1);
	}
	
	return 0;
}














