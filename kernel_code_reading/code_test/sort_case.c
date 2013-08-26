/*
 * =====================================================================================
 *
 *       Filename:  sort_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2013 02:39:00 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>

#include "sort.h"

int int_comp(const void* a, const void* b)
{
	const int* ap = a;
	const int* bp = b;

	return *ap - *bp;
}

void int_sort()
{
	int i;

	int A[10] = {10, 3, 32, 0, 89, 28, 37, 47, 6, 5};

	sort(A, 10, 4, int_comp, NULL);

	printf("== Result after sorted ==\n");
	for(i = 0; i < 10; i++) {
		printf("%d ", A[i]);
	}
	printf("\n");
}

int int_pointer_comp(const void* a, const void* b)
{
	int* const * app = a;
	int* const * bpp = b;
	const int*  ap = *app;
	const int*  bp = *bpp;

	return *ap - *bp;
}

void int_pointer_sort()
{
	int i;

	int A[10] = {10, 3, 32, 0, 89, 28, 37, 47, 6, 5};
	int *pA[10];

	for (i=0; i<10; i++)
		pA[i] = A + i;

	for (i=0; i<10; i++)
		printf("%d\n", *pA[i]);

	sort(pA, 10, 4, int_pointer_comp, NULL);

	printf("== Result after sorted ==\n");
	for(i = 0; i < 10; i++)
		printf("%d ", *pA[i]);
	printf("\n");

	printf("== The original array A[] ==\n");
	for(i = 0; i < 10; i++)
		printf("%d ", A[i]);

	printf("\n");
}

int main()
{
	int_pointer_sort();
	return 0;
}
