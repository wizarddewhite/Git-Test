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

int main()
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

	return 0;
}
