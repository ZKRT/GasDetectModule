#include "rs485.h"
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "can.h"
#include "led.h"
#include "delay.h"
#include "string.h"
#include "poison.h"
#include "timer.h"



void RCC_Configuration(void)
{
	uint32_t StartUpCounter = 0, HSIStatus = 0;
	
	RCC_DeInit();//RCC��λ
	RCC_HSICmd(ENABLE);//HSIʹ��
	
	while((RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) && (StartUpCounter != HSI_STARTUP_TIMEOUT))//�ȴ�HSIʹ�ܽ���
	{
		StartUpCounter++;
	}
	
	if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)//���ʹ�ܳɹ���
	{
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		HSIStatus = (uint32_t)0x00;
	}	
	
	if (HSIStatus == (uint32_t)0x01)//���HSIʹ�ܳɹ�
  {
    /* Enable Prefetch Buffer */
		FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK=SYSCLK
		RCC_PCLKConfig(RCC_HCLK_Div1);  //PCLK=HCLK
		
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//HSI����Ƶ����12��Ƶ���õ�48M��PLL
		
		RCC_PLLCmd(ENABLE);//ʹ��PLLʱ��

		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)//�ȴ�PLLʹ�ܽ���
		{
		}
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//����PLL��ΪSYS��ʱ��Դ

		while (RCC_GetSYSCLKSource() != (uint8_t)RCC_CFGR_SWS_PLL)//�ȴ��������
		{
		}
  }
  else
  {
		//say something else!
  }  
}


uint16_t _model;
uint8_t _type;
uint16_t _range;
uint8_t _uint;
uint8_t _decimal;
uint16_t _meaval;
uint16_t wait_time=0;
uint8_t init_time=40;
uint16_t sen_val_buff[4];
// = {0x0010, 0x0100, 0x1000, 0x0001};
void GetCmd(uint8_t *cmd)
{

}



RCC_ClocksTypeDef RCC_Clocks;

uint8_t senddata[]={0x04,0x20,0x00,0x29,0xc1};
uint8_t receivedata[7];
uint16_t valuedata=0;
uint16_t valuedata2=0;
uint16_t valuecounter=0;
uint16_t valuecounter2=0;
uint16_t sendcounter=0;
uint16_t sendcounter2=0;

extern uint8_t receiveValue[14];
extern uint8_t  receiveFlag;
int main(void )
{		
   SystemInit();		/*ϵͳ��ʼ��*/
   RCC_Configuration();
   RS485_Init(9600);
   delay_init(48);
   TIM3_Int_Init();
	 CAN_Mode_Init(CAN_Mode_Normal);
   LED_Init();
   delay_xms(15000);//�ϵ�һ��ʱ��֮�󣬲���ȥ���������Ƿ���ڣ���Ȼ������ʴ˴����һ����ʱ
	 CheckSensorType(&sensor_type);
		while (1)
		{
			if(loop_flag)   //1Sѭ��һ��
      {
			  loop_flag=0;

		 	  ReadAllVal(sensor_type,sen_val_buff);//�ϵ�Լ23��֮����ܿ�ʼ����CO2������
				zkrt_read_posion_ack();
      }
		}
	
}
