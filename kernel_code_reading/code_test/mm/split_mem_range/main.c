#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split_mem_range.h"

struct map_range mr[NR_RANGE_MR];

void normal_test()
{
	int nr_range;

	memset(mr, 0, sizeof(mr));
	page_size_mask = (1 << PG_LEVEL_1G) | (1 << PG_LEVEL_2M);
	nr_range = split_mem_range(mr, 0, 0, 0x1000);
	nr_range = split_mem_range(mr, nr_range, 0x40000000, 0x80000000);
	nr_range = split_mem_range(mr, nr_range, 0x80000000, 0x90000000);
}

void merge_test1()
{
	int nr_range;

	memset(mr, 0, sizeof(mr));
	page_size_mask = (1 << PG_LEVEL_1G) | (1 << PG_LEVEL_2M);
	nr_range = split_mem_range(mr, 0, 0, 0x1000);
	nr_range = split_mem_range(mr, nr_range, 0x1000, 0x2000);
}

void merge_test2()
{
	int nr_range;

	memset(mr, 0, sizeof(mr));
	page_size_mask = (1 << PG_LEVEL_1G) | (1 << PG_LEVEL_2M);
	nr_range = split_mem_range(mr, 0, 0x1000, 0x2000);
	nr_range = split_mem_range(mr, nr_range, 0, 0x1000);
}

int main()
{
	merge_test2();
	return 0;
}
