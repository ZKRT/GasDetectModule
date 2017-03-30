#include "stm32f0xx_tim.h"
#include "stm32f0xx.h"
#include "led.h"
#include "timer.h"

uint8_t loop_flag=0;
uint8_t num=0;

volatile uint8_t led_rx_count = 0;
volatile uint8_t led_tx_count = 0;

void TIM3_Int_Init(void)
{
   uint16_t arr, psc;
   RCC_ClocksTypeDef RCC_Clocks;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
   RCC_GetClocksFreq(&RCC_Clocks);
   psc = RCC_Clocks.PCLK_Frequency / _10KHZ;//48000000/10000=4800
   arr = _10KHZ / _10HZ; //1000
   
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
   TIM_TimeBaseInitStructure.TIM_Period = arr - 1; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc - 1;  //��ʱ����Ƶ    
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) 
	{
      num = num+1;
      if(num == 15)
			{
				num=0;
        loop_flag = 1;
			}
			
		if (led_rx_count > 0)
		{
			led_rx_count--;
		}
		else
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
		}
		if (led_tx_count > 0)
		{
			led_tx_count--;
		}
		else
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_6);
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
}




