/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2015 11:11:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include "slub.h"

/* size is much smaller than slub_max_order and it is a fraction */
void calculate_order_test1()
{
	int order;
	int size = 512;

	/* no meta data */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 0);
	order = calculate_order(size, 0);
	printf("# Order is %d \n\n\n", order);

	/* small meta data */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 10);
	order = calculate_order(size, 10);
	printf("# Order is %d \n\n\n", order);

	/* meta data is half of size */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 256);
	order = calculate_order(size, 256);
	printf("# Order is %d \n\n\n", order);

	/* meta data equals to size */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, size);
	order = calculate_order(size, size);
	printf("# Order is %d \n\n\n", order);
}

/* size equals to slub_max_order */
void calculate_order_test2()
{
	int order;
	int size = PAGE_SIZE << 3;

	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 10);
	order = calculate_order(size, 10);
	printf("# Order is %d \n\n\n", order);
}

/* size is much smaller than slub_max_order, but it is a not fraction */
void calculate_order_test3()
{
	int order;
	int size = 371;

	/* no meta data */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 0);
	order = calculate_order(size, 0);
	printf("# Order is %d \n\n\n", order);

	/* small meta data */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 10);
	order = calculate_order(size, 10);
	printf("# Order is %d \n\n\n", order);

	/* meta data is half of size */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, 256);
	order = calculate_order(size, 256);
	printf("# Order is %d \n\n\n", order);

	/* meta data equals to size */
	printf("# Calculate Order of 0x%x byte with 0x%x meta\n", size, size);
	order = calculate_order(size, size);
	printf("# Order is %d \n\n\n", order);
}

int main()
{
	calculate_order_test3();
	return 0;
}
