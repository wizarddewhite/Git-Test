/*
 * =====================================================================================
 *
 *       Filename:  bit_ops.c
 *
 *    Description:  This file focus on common bit operations.
 *
 *        Version:  1.0
 *        Created:  01/24/2014 10:30:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdbool.h>

bool is_power2(int x)
{
	if (x == 0)
		return false;

	if ((x & (x-1)) == 0)
		return true;
	else
		return false;
}

void test_is_power2()
{
	printf("%d is%spower of 2\n",
			2, is_power2(2)?" ":" not ");
	printf("%d is%spower of 2\n",
			5, is_power2(5)?" ":" not ");
	printf("%d is%spower of 2\n",
			8, is_power2(8)?" ":" not ");
	printf("%d is%spower of 2\n",
			16, is_power2(16)?" ":" not ");
	printf("%d is%spower of 2\n",
			96, is_power2(96)?" ":" not ");
}

int main()
{
	test_is_power2();
	return 0;
}
