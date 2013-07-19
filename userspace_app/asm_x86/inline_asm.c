/*
 * =====================================================================================
 *
 *       Filename:  inline_asm.c
 *
 *    Description:  This code is copied from 
 *                  http://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
 *
 *        Version:  1.0
 *        Created:  07/18/2013 03:25:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "asm_string.h"
#include "asm_math.h"

void strcpy_test()
{
	char a[] = "This is a happy world";
	char b[25] = {0};

	asm_strcpy(b, a);

	printf("%s\n", b);
}

void math_test()
{
	int a = 10, b = 15;

	printf("a+b = %d\n", asm_add(a, b));
}

int main(void)
{
	math_test();

	return 0;
}
