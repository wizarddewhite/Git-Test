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
	printf("range [0x30, 0x30+0x10] is%smemory region\n",
		memblock_is_region_memory(0x30, 0x10)?" ": "n't ");
	printf("range [0x30, 0x30+0x11] is%smemory region\n",
		memblock_is_region_memory(0x30, 0x11)?" ": "n't ");
	printf("range [0x2f, 0x2f+0x11] is%smemory region\n",
		memblock_is_region_memory(0x30, 0x11)?" ": "n't ");
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

void alloc_free_test()
{
	phys_addr_t addr;
	memblock_debug = 1;

	/* First add some range in memory */
	memblock_add_range(&memblock.memory, 0x1000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x3000, 0x1000, 0, 0);
	memblock_dump_all();

	/* Allocate 0x100 */
	addr = memblock_alloc(0x100, 0x100);
	printf("--allocated address is %#lx\n", addr);
	memblock_dump_all();

	/* Allocate 0x100 with 0x400 alignment */
	addr = memblock_alloc(0x100, 0x400);
	printf("--allocated address is %#lx\n", addr);
	memblock_dump_all();

	/* Free the last one */
	memblock_free(addr, 0x100);
	memblock_dump_all();

}

void isolate_test()
{
	int start_rgn, end_rgn;
	memblock_debug = 1;

	/* First add some range in memory */
	memblock_add_range(&memblock.memory, 0x1000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x3000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x6000, 0x1000, 0, 0);
	memblock_dump_all();

	memblock_isolate_range(&memblock.memory, 0x3500, 0x500,
			&start_rgn, &end_rgn);
	printf("start: %d end: %d\n", start_rgn, end_rgn);
	memblock_dump_all();

}

void next_valid_test()
{
	unsigned long pfn;

	// page aligned memory region
	memblock_add_range(&memblock.memory, 0x1000, 0x1000, 0, 0);
	memblock_add_range(&memblock.memory, 0x5000, 0x1000, 0, 0);
	// none page aligned memory region
	memblock_add_range(&memblock.memory, 0x5000, 0x1100, 0, 0);
	memblock_add_range(&memblock.memory, 0x6101, 0x1000, 0, 0);

	printf("%s: dumping memory\n", __func__);
	memblock_dump(&memblock.memory, "test");

	for (pfn = 1; pfn < 10; pfn++) {
		printf("pfn: %lx\n", pfn);
		pfn = memblock_next_valid_pfn(pfn, 10) - 1;
	}
}

int main()
{
	// add_test();
	// search_test();
	// remove_test();
	// alloc_free_test();
	isolate_test();
	// next_valid_test();
	return 0;
}
