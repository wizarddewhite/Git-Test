/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2015 11:11:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "memblock.h"

void add_test()
{
	memblock_add_range(&memblock.memory, 0x10, 0x10, 0, 0);
	memblock_add_range(&memblock.memory, 0x30, 0x10, 0, 0);
	memblock_add_range(&memblock.memory, 0x05, 0x10, 0, 0);
	printf("%s: dumping memory\n", __func__);
	memblock_dump(&memblock.memory, "test");
}

void search_test()
{
	add_test();
	printf("addr 0x31 is at index: %d\n", memblock_search(&memblock.memory, 0x31));
	printf("addr 0x41 is at index: %d\n", memblock_search(&memblock.memory, 0x41));
}

void remove_test()
{
	memblock_add_range(&memblock.memory, 0x1000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x3000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x5000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x7000, 0x1000, 0, 0);
	memblock_dump(&memblock.memory, "test");

	memblock_remove_range(&memblock.memory, 0x1200, 0x100);
	memblock_dump(&memblock.memory, "test");
}

int main()
{
	remove_test();
	return 0;
}
