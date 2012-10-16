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

struct kfifo * queue_init_test(int size)
{
	struct kfifo *queue;
	char         *buffer;

	queue  = malloc(sizeof(struct kfifo));
	buffer = malloc(size);

	kfifo_init(queue, buffer, size);

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

void queue_status_test()
{
	struct kfifo *queue = queue_init_test(1024);

	printf("queue size is %d\n", kfifo_size(queue));
	printf("queue available %d\n", kfifo_avail(queue));
	printf("queue unused %d\n", kfifo_unused(queue));
	printf("used number is %d\n", kfifo_len(queue));
	printf("queue is empty: %s\n", kfifo_is_empty(queue)?"yes":"no");

	return;
}

void queue_in_test()
{
	struct kfifo *queue = queue_init_test(16);
	int i = 0;

	for (i = 0; i < 32; i++)
		kfifo_in(queue, &i, sizeof(i));

	printf("queue size is %d\n", kfifo_size(queue));
	printf("queue available %d\n", kfifo_avail(queue));
	printf("used number is %d\n", kfifo_len(queue));
	printf("queue is empty: %s\n", kfifo_is_empty(queue)?"yes":"no");

	return;
}

int main() 
{
	queue_in_test();
	return 0;
}
