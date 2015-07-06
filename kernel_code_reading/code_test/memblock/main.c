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

#define INIT_MEMBLOCK_REGIONS	128
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS];
struct memblock_type memory = {
	.regions 	= memblock_reserved_init_regions,
	.cnt		= 1,
	.max		= INIT_MEMBLOCK_REGIONS,
};

int main()
{
	memblock_add_range(&memory, 0x10, 0x10, 0, 0);
	memblock_add_range(&memory, 0x30, 0x10, 0, 0);
	memblock_add_range(&memory, 0x05, 0x10, 0, 0);
	printf("%s: dumping memory\n", __func__);
	memblock_dump(&memory, "test");
	return 0;
}
