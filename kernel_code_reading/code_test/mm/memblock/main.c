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

#define MEMBLOCK_ALLOC_ANYWHERE	(~(phys_addr_t)0)
#define MEMBLOCK_ALLOC_ACCESSIBLE	0

#define INIT_MEMBLOCK_REGIONS	128
static struct memblock_region memblock_memory_init_regions[INIT_MEMBLOCK_REGIONS] ;
static struct memblock_region memblock_reserved_init_regions[INIT_MEMBLOCK_REGIONS] ;
#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
static struct memblock_region memblock_physmem_init_regions[INIT_PHYSMEM_REGIONS] ;
#endif

struct memblock memblock = {
	.memory.regions		= memblock_memory_init_regions,
	.memory.cnt		= 1,	/* empty dummy entry */
	.memory.max		= INIT_MEMBLOCK_REGIONS,

	.reserved.regions	= memblock_reserved_init_regions,
	.reserved.cnt		= 1,	/* empty dummy entry */
	.reserved.max		= INIT_MEMBLOCK_REGIONS,

#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
	.physmem.regions	= memblock_physmem_init_regions,
	.physmem.cnt		= 1,	/* empty dummy entry */
	.physmem.max		= INIT_PHYSMEM_REGIONS,
#endif

	.bottom_up		= false,
	.current_limit		= MEMBLOCK_ALLOC_ANYWHERE,
};

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
