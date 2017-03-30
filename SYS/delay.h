#ifndef __DELAY_H__
#define __DELAY_H__
#include "stm32f0xx.h"
void delay_init(uint8_t SYSCLK);
void delay_us(uint32_t nus);
void delay_xms(uint16_t nms);

#endif


