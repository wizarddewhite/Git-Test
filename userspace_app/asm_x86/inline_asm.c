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

void asm_add(void)
{
        int foo = 10, bar = 15;
        __asm__ __volatile__("addl  %%ebx,%%eax\n"
                             :"=a"(foo)
                             :"a"(foo), "b"(bar)
                             );
        printf("foo+bar=%d\n", foo);
}

void asm_assign(void)
{
	int a = 10, b = 0;

	printf("b is %d at first\n", b);

	__asm__ __volatile__("movl %1, %%eax;\n"
			     "movl %%eax, %0;\n"
			     :"=r"(b)     /* output */
			     :"r"(a)      /* input */
			     :"%eax"      /* clobbered register */
			     );

	printf("b is %d now\n", b);
}

int main(void)
{
	asm_add();

	asm_assign();

	return 0;
}
