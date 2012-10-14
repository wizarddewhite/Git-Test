/*
 * =====================================================================================
 *
 *       Filename:  kfifo.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/19/2012 09:44:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include "kfifo.h"
/* Given a _kfifo structure, initialize it with a given buffer and size 
 * This is called in marco kfifo_init
 */
int __kfifo_init(struct __kfifo *fifo, void *buffer,
		unsigned int size, unsigned int esize)
{
	size /= esize;

	/* in the original code, size will be rounddown to power of 2 */
	//if (!is_power_of_2(size))
	//	size = rounddown_pow_of_two(size);

	fifo->in = 0;
	fifo->out = 0;
	fifo->esize = esize;
	fifo->data = buffer;

	if (size < 2) {
		fifo->mask = 0;
		return -1;
	}
	fifo->mask = size - 1;

	return 0;
}

unsigned int __kfifo_max_r(unsigned int len, unsigned int recsize)
{
	unsigned int max = (1 << (recsize << 3)) - 1;

	if (len > max)
		return max;
	return len;
}
