#ifndef __USART_H
#define	__USART_H

#include "stm32f0xx.h"
#include <stdio.h>

void USART1_Config(uint32_t bound);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);

#endif 




