/*
 * =====================================================================================
 *
 *       Filename:  getcontext.c
 *       http://publib.boulder.ibm.com/infocenter/zos/v1r11/index.jsp?topic=/com.ibm.zos.r11.bpxbd00/rgcntx.htm
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 06:50:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ucontext.h>

int  x = 0;
ucontext_t context, *cp = &context;

void func(void)
{
	x++;
	setcontext(cp);
}

int main(void)
{
	getcontext(cp);
	printf("Start from here--- x == %d\n", x);
	if (!x) {
		printf("getcontext has been called\n");
		func();
	}
	else {
		printf("setcontext has been called\n");
	}
	return 0;
}
