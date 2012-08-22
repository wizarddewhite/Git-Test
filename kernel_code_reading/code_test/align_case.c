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

int main()
{
	calculate_mem_align();
	return 0;
}
