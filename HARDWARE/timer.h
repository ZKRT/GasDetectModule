#ifndef _TIMER_H
#define _TIMER_H
#include "stm32f0xx.h"

#define _10KHZ 10000
#define _10HZ 10
#define _1HZ 1

extern volatile uint8_t led_rx_count;
extern volatile uint8_t led_tx_count;

extern uint8_t loop_flag;
void TIM3_Int_Init(void);

#endif
