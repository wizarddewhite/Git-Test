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

int main() 
{
	struct kfifo *queue = queue_init_test(1024);
	return 0;
}
