#include "rs485.h"
#include "delay.h"
#include "queue.h"
#include "string.h"
#include "poison.h"

#if EN_USART1_RX   		//如果使能了接收   	  
//接收缓存区 	
QUEUE rs485_queue;
#define QUEUE_MAX_SIZE 64
uint8_t RS485_RX_BUF[QUEUE_MAX_SIZE];  	//接收缓冲,最大64个字节.

/**
  * @brief 串口1中断函数
  * @param  
  * @retval 
  */

uint8_t  receiveValue[7];
uint8_t  receiveFlag=0;
uint8_t  receiveCounter=0;
	
void USART1_IRQHandler(void)
{
	uint8_t res;	    
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)//接收到数据
	{	 	
	  res =USART_ReceiveData(USART1);//;读取接收到的数据USART1->DR
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
  * @brief  rs485 初始化函数
  * @param  bound 波特率
  * @retval 
  */
void RS485_Init(uint32_t bound)
{
   GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	
  //串口1引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1); //GPIOA10复用为USART1
	
	//USART1    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	
	//PA8推挽输出，485模式控制  
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure); 
	

   //USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;//字长为9位数据格式,八位会不正常
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;//偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
   USART_Cmd(USART1, ENABLE);  //使能串口 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart1 NVIC 配置
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif	

	 RS485_RX_EN;				//默认为接收模式	
   queue_init_static(&rs485_queue,RS485_RX_BUF,QUEUE_MAX_SIZE); //初始化队列
}


/**
  * @brief  485 发送数据
  * @param  buff 发送数组指针，len 数组长度
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
  * @brief  485 接收数据
  * @param  buff 接收数据数组 ，准备接收长度
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






