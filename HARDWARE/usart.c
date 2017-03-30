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
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
	
  //串口1引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_0); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_0); //GPIOA10复用为USART1
	
	//USART2    
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10
	
	//PA8推挽输出，485模式控制  
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //GPIOA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA8
	

   //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART1, ENABLE);  //使能串口 1
	
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART1_RX	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif	
	
	RS485_TX_EN=0;				//默认为接收模式	
   
   
   
   
   
   
   
   
   
   
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