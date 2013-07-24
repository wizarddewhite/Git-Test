/*
 * =====================================================================================
 *
 *       Filename:  stat.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2013 08:20:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
	struct stat st;

	if (stat("makefile", &st))
		return -1;

	printf("the size of makefile is %ld\n", st.st_size);

	return 0;
}
