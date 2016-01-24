/*
 * =====================================================================================
 *
 *       Filename:  asm_math.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/19/2013 11:06:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>


int asm_add(int a, int b)
{
        __asm__ __volatile__("addl  %%ebx,%%eax\n"
                             :"=a"(a)
                             :"a"(a), "b"(b)
			     :"memory"
                             );
	return a;
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

