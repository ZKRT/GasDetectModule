/*
 ***************(C) COPYRIGHT 2014 DONG STUDIO **********
 *	@file			: def.h
 *	@description	: some common definitions
 *	@platform		: stm32f4 flight control board
 *	@author			: Dong
 *********************************************************
 */

#ifndef __DEF_H
#define __DEF_H

#include <stdint.h>

#ifndef BOOL
#define BOOL	uint16_t
#define TRUE	1
#define FALSE	0
#endif

#define NULL	0

#define PI	3.14159265f

#define EARTH_GRAVITY	9.812f
#define EARTH_RADIUS	6.371393e6f
#define EARTH_MAGNETIC	0.325f

#define LOBYTE(x)	(uint8_t)(0x00ff & (x))
#define HIBYTE(x)	(uint8_t)(0x00ff & ((x) >> 8))
#define BYTE0(x)	(*(uint8_t *)&x)
#define BYTE1(x)	(*((uint8_t *)&x+1))
#define BYTE2(x)	(*((uint8_t *)&x+2))
#define BYTE3(x)	(*((uint8_t *)&x+3))
#define MAKEWORD(high, low) (((uint16_t)(high)<<8) | (uint16_t)(low))

#define ABS(x) ((x)>=0?(x):(-(x)))

#define INPI(x) { if (x>PI) x-=2*PI; else if (x<-PI) x += 2*PI; }

#define MAX_LIMIT(x, max) { if (x > max) x = max; }
#define MIN_LIMIT(x, min) { if (x < min) x = min; }
#define CONSTRAIN(x, min, max) { if (x<min) x = min; else if (x>max) x = max; }

/* queue operation definition */
typedef struct {
	uint8_t *buffer;
	uint16_t max_size;
	uint16_t front, rear;
} QUEUE;

void queue_init(QUEUE *queue, uint16_t max_size);
void queue_init_static(QUEUE *queue, uint8_t *buffer, uint16_t max_size);
BOOL queue_push(QUEUE *queue, uint8_t ch);
BOOL queue_push_word(QUEUE *queue, uint16_t word);
BOOL queue_push_data(QUEUE *queue, uint8_t *data, uint16_t size);
BOOL queue_pop(QUEUE *queue, uint8_t *ch);
BOOL queue_pop_word(QUEUE *queue, uint16_t *word);
BOOL queue_pop_data(QUEUE *queue, uint8_t *data, uint16_t size);

BOOL queue_peek(QUEUE *queue, uint8_t *ch);

BOOL queue_isempty(QUEUE *queue);
BOOL queue_isfull(QUEUE *queue);
uint16_t queue_size(QUEUE *queue);

/* check sum function */
uint16_t check_sum(uint8_t *buffer, uint16_t size);
uint8_t check_sum1(uint8_t *buffer, uint16_t size);
uint16_t check_sum2(uint8_t *buffer, uint16_t size);
uint16_t check_crc(uint8_t *buffer, uint16_t size);

#endif

/* End of file */
