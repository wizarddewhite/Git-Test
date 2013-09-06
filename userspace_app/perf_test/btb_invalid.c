/*
 * =====================================================================================
 *
 *       Filename:  btb_invalid.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/06/2013 04:45:50 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h> 
#include <stdlib.h> 

void foo()
{
	int i,j;
	for(i=0; i< 10; i++)
		j+=2;
}

int main(void)
{
	int i;
	for(i = 0; i< 100000000; i++)
		foo();
	return 0;
}
