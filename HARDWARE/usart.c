/*
  ******************************************************************************
  * @file    uart.c 
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  * USART1_TX<----->PA9
  * USART1_RX<----->PA10
  ******************************************************************************
  */
  
#include "usart.h"

 /**
  * @brief  USART1 ??????
  * @param  ?
  * @retval ?
  */
static void USART1_NVIC_Config(void)
{
   NVIC_InitTypeDef NVIC_InitStructure; 
   /* Enable the USART1 Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
}

 
 /**
  * @brief  USART1 GPIO ??,???????115200 8-N-1
  * @param  ?
  * @retval ?
  */
void USART1_Config(uint32_t bound)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	
  //����1���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_0); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_0); //GPIOA10����ΪUSART1
	
	//USART2    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10
	
	//PA8���������485ģʽ����  
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA8
	

   //USART2 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ��� 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif	
	
	RS485_TX_EN=0;				//Ĭ��Ϊ����ģʽ	
   
   
   
   
   
   
   
   
   
   
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;

   /* config USART1 clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
   
   RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE); 
   /*????????*/
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
   GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);  
   /* USART1 GPIO config */
   /* Configure USART1 Tx (PA.09) as alternate function push-pull */
   GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_9 ;
   GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);    
   /* Configure USART1 Rx (PA.10) as input mode */
   GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_10 ;
   GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);


   /* USART1 mode config */
   USART_InitStructure.USART_BaudRate = bound;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits = USART_StopBits_1;
   USART_InitStructure.USART_Parity = USART_Parity_No ;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
   USART_Init(USART1, &USART_InitStructure);

//   USART_SetReceiverTimeOut(USART1,1);
//   USART_ReceiverTimeOutCmd(USART1,ENABLE);

   USART_ITConfig(USART1,USART_IT_RTO, ENABLE);	
   USART_Cmd(USART1, ENABLE);
   USART_ClearFlag(USART1, USART_IT_TC);     
   USART1_NVIC_Config();
}


 /**
  * @brief  ???c???printf?USART1
  * @param  
  * @retval 
  */
int fputc(int ch, FILE *f)
{
   USART_SendData(USART1, (uint8_t) ch);
   while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
   return (ch);
}


 /**
  * @brief  ???c???scanf?USART1
  * @param  
  * @retval 
  */
int fgetc(FILE *f)
{

	 while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
	 return (int)USART_ReceiveData(USART1);
}