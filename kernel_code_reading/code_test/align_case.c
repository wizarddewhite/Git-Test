/*
 * =====================================================================================
 *
 *       Filename:  align_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2012 03:36:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "kernel.h"

void common_test()
{
	int res;

	res = ALIGN(4, 4);
	printf("ALIGN(4, 4) = %d\n", res);

	res = ALIGN(5, 4);
	printf("ALIGN(5, 4) = %d\n", res);

	res = ALIGN(4, 8);
	printf("ALIGN(4, 8) = %d\n", res);

	res = ALIGN(7, 8);
	printf("ALIGN(7, 8) = %d\n", res);

	res = ALIGN(9, 8);
	printf("ALIGN(9, 8) = %d\n", res);

	/* what will happen if the align is not power of 2? */
	res = ALIGN(9, 7);
	printf("ALIGN(9, 7) = %d\n", res);
}

void calculate_mem_align()
{
	unsigned int aligns[12]; /* Alignments from 1Mb to 2Gb */
	unsigned int min_align, align, align1, size;
	int order;

	for (order = 0; order <= 11; order++) {
		aligns[order] = 0;
	}
	aligns[0] = 5 * (1 << 20); /* 5*1M = 5M */
	aligns[2] = 3 * (1 << 22); /* 3*4M = 12M */
	aligns[5] = 2 * (1 << 25); /* 2*32M = 64M */

	align = 0;
	min_align = 0;

	for (order = 0; order <= 5; order++) {
		align1 = 1;
		align1 <<= (order + 20);

		size = ALIGN(align + min_align, min_align);
		if (!align)
			min_align = align1;
		else if (ALIGN(align + min_align, min_align) < align1)
			min_align = align1 >> 1;
		align += aligns[order];
	}
}

unsigned long long pci_size(unsigned long long base, 
		unsigned long long maxbase, 
		unsigned long long mask)
{
	unsigned long long size = mask & maxbase;	/* Find the significant bits */
	if (!size)
		return 0;

	/* Get the lowest of them to find the decode size, and
	   from that the extent.  */
	size = (size & ~(size-1)) - 1;

	/* base == maxbase can be valid only if the BAR has
	   already been programmed with all 1s.  */
	if (base == maxbase && ((base | size) & mask) != mask)
		return 0;

	return size;
}

unsigned long long size_clip(unsigned long long size)
{
	size = (size & ~(size-1)) - 1;

	return size;
}

/* this case verified the pci bar size must be power of 2 */
void pci_size_test()
{
	printf("size_clip of 0x10000 is %llx\n", size_clip(0x10000));
	printf("size_clip of 0x10001 is %llx\n", size_clip(0x10001));
	printf("size_clip of 0x10010 is %llx\n", size_clip(0x10010));
	printf("size_clip of 0x10100 is %llx\n", size_clip(0x10100));
}

int main()
{
	common_test();

	pci_size_test();

	return 0;
}
