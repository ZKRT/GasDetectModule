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
	
	RCC_DeInit();//RCC复位
	RCC_HSICmd(ENABLE);//HSI使能
	
	while((RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) && (StartUpCounter != HSI_STARTUP_TIMEOUT))//等待HSI使能结束
	{
		StartUpCounter++;
	}
	
	if (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) != RESET)//如果使能成功了
	{
		HSIStatus = (uint32_t)0x01;
	}
	else
	{
		HSIStatus = (uint32_t)0x00;
	}	
	
	if (HSIStatus == (uint32_t)0x01)//如果HSI使能成功
  {
    /* Enable Prefetch Buffer */
		FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);//HCLK=SYSCLK
		RCC_PCLKConfig(RCC_HCLK_Div1);  //PCLK=HCLK
		
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);//HSI二分频，再12倍频，得到48M的PLL
		
		RCC_PLLCmd(ENABLE);//使能PLL时钟

		while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)//等待PLL使能结束
		{
		}
		
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);//配置PLL作为SYS的时钟源

		while (RCC_GetSYSCLKSource() != (uint8_t)RCC_CFGR_SWS_PLL)//等待配置完成
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
   SystemInit();		/*系统初始化*/
   RCC_Configuration();
   RS485_Init(9600);
   delay_init(48);
   TIM3_Int_Init();
	 CAN_Mode_Init(CAN_Mode_Normal);
   LED_Init();
   delay_xms(15000);//上电一定时间之后，才能去读传感器是否存在，不然会出错，故此处需加一个延时
	 CheckSensorType(&sensor_type);
		while (1)
		{
			if(loop_flag)   //1S循环一次
      {
			  loop_flag=0;

		 	  ReadAllVal(sensor_type,sen_val_buff);//上电约23秒之后才能开始读到CO2的数据
				zkrt_read_posion_ack();
      }
		}
	
}
