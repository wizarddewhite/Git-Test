/*
 * =====================================================================================
 *
 *       Filename:  queue_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2012 09:52:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "kfifo.h"

struct kfifo * queue_init_test(void *buf, int size)
{
	struct kfifo *queue;
	char         *buffer;

	queue  = malloc(sizeof(struct kfifo));

	kfifo_init(queue, buf, size);

	/* After this the "queue" is initialized like this
	 * queue
	 *   kfifo
	 *      in = 0
	 *      out = 0
	 *      mask = 1023
	 *      esize = 1
	 *      data = buffer
	 */

	return queue;
}

void queue_show_status(struct kfifo *queue)
{
	printf("queue size is %d\n", kfifo_size(queue)); // buffer size
	printf("queue available %d\n", kfifo_avail(queue)); // buffer available
	printf("queue unused %d\n", kfifo_unused(&queue->kfifo)); // buffer unused, same as above?
	printf("used number is %d\n", kfifo_len(queue)); // buffer used
	printf("queue is empty: %s\n", kfifo_is_empty(queue)?"yes":"no");
}

void queue_status_test()
{
	char  *buffer = malloc(1024);
	struct kfifo *queue = queue_init_test(buffer, 1024);

	queue_show_status(queue);

	return;
}

void queue_in_test()
{
	int i ;
	char  *buffer; 
	struct kfifo *queue; 

	i = 16;

	buffer = malloc(i * sizeof(i));
	queue = queue_init_test(buffer, i * sizeof(i));

	for (i = 0; i < 32; i++)
		kfifo_in(queue, &i, sizeof(i));

	printf("queue size is %d\n", kfifo_size(queue));
	printf("queue available %d\n", kfifo_avail(queue));
	printf("used number is %d\n", kfifo_len(queue));
	printf("queue is empty: %s\n", kfifo_is_empty(queue)?"yes":"no");

	return;
}

void queue_out_test()
{
	int i , tmp;
	char  *buffer; 
	struct kfifo *queue; 

	i = 16;

	buffer = malloc(i * sizeof(i));
	queue = queue_init_test(buffer, i * sizeof(i));

	for (i = 0; i < 16; i++)
		kfifo_in(queue, &i, sizeof(i));

	queue_show_status(queue);

	for (i = 0; i < 6; i++)
	{
		kfifo_out(queue, &tmp, sizeof(tmp));
		printf("%d\n", tmp);
	}

	queue_show_status(queue);

	return;
}

int main() 
{
	queue_out_test();
	return 0;
}
