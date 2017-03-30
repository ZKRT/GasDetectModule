#include <stdlib.h>

#include "queue.h"

void queue_init_static(QUEUE *queue, uint8_t *buffer, uint16_t max_size) {
	queue->buffer = buffer;
	queue->max_size = max_size;
	queue->front = queue->rear = 0;
}

BOOL queue_push(QUEUE *queue, uint8_t ch) {
	if (queue_isfull(queue)) return FALSE;
	queue->buffer[queue->rear++] = ch;
	if (queue->rear == queue->max_size) queue->rear = 0;
	return TRUE;
}

BOOL queue_push_data(QUEUE *queue, uint8_t *buffer, uint16_t size) {
	int i;
	if (queue_size(queue)+size > queue->max_size) return FALSE;
	
	for (i=0; i<size; i++) {
		queue->buffer[queue->rear++] = buffer[i];
		if (queue->rear == queue->max_size) queue->rear = 0;
	}
	
	return TRUE;
}

BOOL queue_pop(QUEUE *queue, uint8_t *ch) {
	if (queue_isempty(queue)) return FALSE;
	*ch = queue->buffer[queue->front++];
	if (queue->front == queue->max_size) queue->front = 0;
	return TRUE;
}

BOOL queue_pop_data(QUEUE *queue, uint8_t *buffer, uint16_t size) {
	int i;
	
	if (queue_size(queue) < size) return FALSE;
	
	for (i=0; i<size; i++) {
		buffer[i] = queue->buffer[queue->front++];
		if (queue->front == queue->max_size) queue->front = 0;
	}

	return TRUE;
}

BOOL queue_peek(QUEUE *queue, uint8_t *ch) {
	if (queue_isempty(queue)) return FALSE;
	*ch = queue->buffer[queue->front];
	return TRUE;
}


BOOL queue_isempty(QUEUE *queue) {
	return queue->rear == queue->front;
}

BOOL queue_isfull(QUEUE *queue) {
	BOOL full =  queue->rear + 1 == queue->front || (queue->front == 0 && queue->rear == queue->max_size-1);
	return full;
}

uint16_t queue_size(QUEUE *queue) {
	uint16_t size;
	if (queue->rear >= queue->front) {
		size = queue->rear - queue->front;
	}
	else {
		size = queue->rear + queue->max_size - queue->front;
	}
	return size;
}
