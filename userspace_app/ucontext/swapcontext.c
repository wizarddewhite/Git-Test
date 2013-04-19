/*
 * =====================================================================================
 *
 *       Filename:  swapcontext.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 04:54:09 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#define _XOPEN_SOURCE_EXTENDED 1
#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <errno.h>

#ifdef _LP64
  #define STACK_SIZE 2097152+16384 /* large enough value for AMODE 64 */
#else
  #define STACK_SIZE 16384         /* AMODE 31 addressing */
#endif

void func();

ucontext_t context, *cp = &context;
ucontext_t old_con;

int main(void)
{
	int  value = 1;
	
	getcontext(cp);
	context.uc_link = &old_con;
	if ((context.uc_stack.ss_sp = (char *) malloc(STACK_SIZE)) != NULL) {
		context.uc_stack.ss_size = STACK_SIZE;
		context.uc_stack.ss_flags = 0;
		errno = 0;
		makecontext(cp,func,1,value);
		if(errno != 0){
			perror("Error reported by makecontext()");
			return -1;         /* Error occurred exit */ 
		}
	}
	else {
		perror("not enough storage for stack");
		abort();
	}
	printf("context has been built\n");
	swapcontext(&old_con, cp);

	printf("come back and continue the task\n");
	return 0;
}

void func(int arg) 
{
	printf("function called with value %d\n",arg);
	return;
}
