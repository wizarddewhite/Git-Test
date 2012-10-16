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
/*
 * internal helper to calculate the unused elements in a fifo
 */
unsigned int kfifo_unused(struct __kfifo *fifo)
{
	return (fifo->mask + 1) - (fifo->in - fifo->out);
}

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


static void kfifo_copy_in(struct __kfifo *fifo, const void *src,
		unsigned int len, unsigned int off)
{
	unsigned int size = fifo->mask + 1;
	unsigned int esize = fifo->esize;
	unsigned int l;

	off &= fifo->mask;
	if (esize != 1) {
		off *= esize;
		size *= esize;
		len *= esize;
	}
	l = min(len, size - off);

	memcpy(fifo->data + off, src, l);
	memcpy(fifo->data, src + l, len - l);
	/*
	 * make sure that the data in the fifo is up to date before
	 * incrementing the fifo->in index counter
	 */
	//smp_wmb();
}

unsigned int __kfifo_in(struct __kfifo *fifo,
		const void *buf, unsigned int len)
{
	unsigned int l;

	l = kfifo_unused(fifo);
	if (len > l)
		len = l;

	kfifo_copy_in(fifo, buf, len, fifo->in);
	fifo->in += len;
	return len;
}

#define	__KFIFO_POKE(data, in, mask, val) \
	( \
	(data)[(in) & (mask)] = (unsigned char)(val) \
	)

/*
 * __kfifo_poke_n internal helper function for storeing the length of
 * the record into the fifo
 */
static void __kfifo_poke_n(struct __kfifo *fifo, unsigned int n, int recsize)
{
	unsigned int mask = fifo->mask;
	unsigned char *data = fifo->data;

	__KFIFO_POKE(data, fifo->in, mask, n);

	if (recsize > 1)
		__KFIFO_POKE(data, fifo->in + 1, mask, n >> 8);
}
unsigned int __kfifo_in_r(struct __kfifo *fifo, const void *buf,
		unsigned int len, int recsize)
{
	if (len + recsize > kfifo_unused(fifo))
		return 0;

	__kfifo_poke_n(fifo, len, recsize);

	kfifo_copy_in(fifo, buf, len, fifo->in + recsize);
	fifo->in += len + recsize;
	return len;
}
