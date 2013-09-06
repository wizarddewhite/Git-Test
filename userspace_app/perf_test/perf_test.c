/*
 * =====================================================================================
 *
 *       Filename:  perf_test.c
 *
 *    Description:  http://blog.csdn.net/cybertan/article/details/8139017
 *
 *        Version:  1.0
 *        Created:  09/06/2013 11:26:40 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

				
void longa() 
{ 
	int i,j; 
	for(i = 0; i < 1000000; i++) 
		j=i; //am I silly or crazy? I feel boring and desperate. 
	i = j;
} 

void foo2() 
{ 
	int i; 
	for(i=0 ; i < 10; i++) 
		longa(); 
} 

void foo1() 
{ 
	int i; 
	for(i = 0; i< 100; i++) 
		longa(); 
} 

int main(void) 
{ 
	foo1(); 
	foo2(); 

	return 0;
} 


