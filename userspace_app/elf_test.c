/*
 * =====================================================================================
 *
 *       Filename:  elf_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/26/2013 03:51:51 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

/* 
 *  readelf -h elf_test
 *
 *  readelf -S elf_test
 *
 *
 * */
#include <stdio.h>
int global_data = 4;
int global_data_2;

int main(int argc, char **argv){ 
	int local_data = 3;
	printf("Hello Worldn");
	printf("global_data = %d\n", global_data);
	printf("global_data_2 = %d\n", global_data_2);
	printf("local_data = %d\n", local_data);
	return (0);
}
