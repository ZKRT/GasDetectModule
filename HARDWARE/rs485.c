#include "rs485.h"
#include "delay.h"
#include "queue.h"
#include "string.h"
#include "poison.h"

#if EN_USART1_RX   		//���ʹ���˽���   	  
//���ջ����� 	
QUEUE rs485_queue;
#define QUEUE_MAX_SIZE 64
uint8_t RS485_RX_BUF[QUEUE_MAX_SIZE];  	//���ջ���,���64���ֽ�.

/**
  * @brief ����1�жϺ���
  * @param  
  * @retval 
  */

uint8_t  receiveValue[7];
uint8_t  receiveFlag=0;
uint8_t  receiveCounter=0;
	
void USART1_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//���յ�����
	{	 	
	  res =USART_ReceiveData(USART1);//;��ȡ���յ�������USART1->DR
	  //queue_push(&rs485_queue,res);
		receiveValue[receiveCounter++] = res;
		if(receiveCounter>=ControlType)
		{
			receiveCounter = 0;
			receiveFlag = 1;
		}
	}							 
} 
#endif	

/**
  * @brief  rs485 ��ʼ������
  * @param  bound ������
  * @retval 
  */
void RS485_Init(uint32_t bound)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	
  //����1���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); //GPIOA10����ΪUSART1
	
	//USART1    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	//PA8���������485ģʽ����  
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//�ֳ�Ϊ9λ���ݸ�ʽ,��λ�᲻����
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_Even;//żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
   USART_Cmd(USART1, ENABLE);  //ʹ�ܴ��� 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart1 NVIC ����
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif	

	 RS485_RX_EN;				//Ĭ��Ϊ����ģʽ	
   queue_init_static(&rs485_queue,RS485_RX_BUF,QUEUE_MAX_SIZE); //��ʼ������
}


/**
  * @brief  485 ��������
  * @param  buff ��������ָ�룬len ���鳤��
  * @retval 
  */
void RS485_Send_Data(uint8_t *buf,uint8_t len)
{
	uint8_t t;
	RS485_TX_EN			
  rs485_queue.front = rs485_queue.rear;
  USART_ClearFlag(USART1,USART_FLAG_TC);
  for(t=0;t<len;t++)		
	{	
      USART_SendData(USART1,buf[t]); 
      while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
	}	 		
	RS485_RX_EN;				
}

/**
  * @brief  485 ��������
  * @param  buff ������������ ��׼�����ճ���
  * @retval 
  */
void RS485_Receive_Data(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	if(receiveFlag)
	{
		receiveFlag = 0;
		
		for(i=0;i<len;i++)
			buf[i] = receiveValue[i];
		
		for(i=0;i<7;i++)
			receiveValue[i] = 0;
	}
//  	uint8_t i=0;
//   memset(buf,0,len);
//   while(queue_size(&rs485_queue) < len && (i < 20))
//   {
//      delay_xms(1);
//      i++;
//   }
//   if(queue_size(&rs485_queue)==len)
//     queue_pop_data(&rs485_queue,buf,len);
//   else
//     rs485_queue.front = rs485_queue.rear;
}






