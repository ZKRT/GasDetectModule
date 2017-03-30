#ifndef __QUEUE_H__
#define __  QUEUE_H__

#include <stdint.h>

#ifndef BOOL
#define BOOL	uint16_t
#define TRUE	1
#define FALSE	0
#endif

#define NULL	0

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

#endif

/* End of file */
