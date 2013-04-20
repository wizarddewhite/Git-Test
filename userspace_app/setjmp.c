/*
 * =====================================================================================
 *
 *       Filename:  setjmp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 09:21:41 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <setjmp.h>

jmp_buf entry;

void raise_exception()
{
	printf("   --- do some other stuff\n");
	longjmp(entry, 1);
}

int main(void)
{
	if (setjmp(entry) == 0)
	{
		printf("return point setup\n");
		raise_exception();
	}
	else
		printf("exception handled\n");

	return 0;
}
