#ifndef __PWM_H
#define	__PWM_H

#include "stm32f0xx.h"
extern uint16_t IndexWave[];
static void TIM3_GPIO_Config(void) ;
static void TIM3_Mode_Config(void);
static void TIM_NVIC_Config(void);
void TIM3_Breathing_Init(void);
void Breathing_Test(void);

#endif 

