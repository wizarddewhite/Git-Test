/*
 * =====================================================================================
 *
 *       Filename:  percpu.c
 *
 *    Description:  this is a test from percpu.c
 *
 *        Version:  1.0
 *        Created:  10/30/2013 10:06:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <math.h>
#include "kernel.h"
#define INT_MAX		((int)(~0U>>1))
#define PAGE_SIZE (4 << 10)
#define PAGE_MASK	(~(PAGE_SIZE-1))
#define PERCPU_DYNAMIC_EARLY_SIZE	(12 << 10)

#define PCPU_MIN_UNIT_SIZE		__ALIGN_KERNEL((32 << 10), PAGE_SIZE)

/* mimic the operation in pcpu_build_alloc_info() 
 * The purpose of this test will tries to understand how upa is calculated
 * and the relationship between upa and atom_size/min_unit_size
 */
size_t upa_calculate(size_t reserved_size, size_t dyn_size, size_t atom_size)
{
	size_t static_size = 0xc1a73040 - 0xc1a6b000; //1 << 14, 16K;
	size_t size_sum, min_unit_size, alloc_size;
	int upa;

	printf("    static_size   0x%x\n", static_size);
	/* size_sum: raw total size */
	size_sum = __ALIGN_KERNEL(static_size + reserved_size + 
			max(dyn_size, (size_t)PERCPU_DYNAMIC_EARLY_SIZE), PAGE_SIZE);
	dyn_size = size_sum - static_size - reserved_size;
	printf("    size_sum      0x%x\n", size_sum);
	printf("    dyn_size      0x%x\n", dyn_size);

	/* min_unit_size: minimum unit size */
	min_unit_size = max_t(size_t, size_sum, PCPU_MIN_UNIT_SIZE);
	printf("    min_unit_size 0x%x\n", min_unit_size);

	/* alloc_size: atom_size aligned for min_unit_size */
	alloc_size = roundup(min_unit_size, atom_size);
	printf("    alloc_size    0x%x\n", alloc_size);

	upa = alloc_size / min_unit_size;
	printf("    upa  = alloc_size / min_size = %d\n", upa);
	printf("    alloc_size%%upa = %d\n", alloc_size%upa);

	return reserved_size + dyn_size + atom_size;
}

/*
 * From this test, we see the upa has a relationship with the atom_size
 *     +  1         0 <= atom_size <= 2*min_unit_size - 1
 *     |
 *     +  2         2*min_unit_size <= atom_size <= 3*min_unit_size - 1
 * upa |
 *     +  3         3*min_unit_size <= atom_size <= 4*min_unit_size - 1
 *     |
 *     +  4         ...
 *     |
 *     ...
 *
 */
void upa_calculation_test()
{
	printf("--- upa_calculation test ---\n");
	printf("--- atom_size is PAGE_SIZE(0x%x) \n", PAGE_SIZE);
	upa_calculate(0, 0, PAGE_SIZE);

	printf("--- atom_size is 0x2000, less than min_unit_size\n");
	upa_calculate(0, 0, 0x2000);

	printf("--- atom_size is 0xb000, less than min_unit_size\n");
	upa_calculate(0, 0, 0xb000);

	printf("--- atom_size is 0xbfff, less than min_unit_size\n");
	upa_calculate(0, 0, 0xbfff);

	printf("--- atom_size is 0xc000, equals to min_unit_size\n");
	upa_calculate(0, 0, 0xc000);

	printf("--- upa_calculation test ---\n");
	printf("--- atom_size is 0xc001, bigger than min_unit_size\n");
	upa_calculate(0, 0, 0xc001);

	printf("--- atom_size is 0xc002, bigger than min_unit_size\n");
	upa_calculate(0, 0, 0xc002);

	printf("--- atom_size is (0x18000-1) , almost two times of min_unit_size\n");
	upa_calculate(0, 0, (0x18000-1));

	printf("--- atom_size is 0x18000, two times of min_unit_size\n");
	upa_calculate(0, 0, 0x18000);

	printf("--- atom_size is (0x18000+1), more than two times of min_unit_size\n");
	upa_calculate(0, 0, 0x18001);

	printf("--- atom_size is 0x24000, three times of min_unit_size\n");
	upa_calculate(0, 0, 0x24000);

	return;
}

/* 
 * This function mimic the middle part for function pcpu_build_alloc_info
 * when calculating the best_upa.
 */
void best_upa_calculation(int group_cnt[], int nr_groups, int upa)
{
	int group;
	int last_allocs;
	int best_upa;
	int nr_units;
	int num_possible_cpus = 0;

	for (group = 0; group < nr_groups; group++)
		num_possible_cpus += group_cnt[group];

	last_allocs = INT_MAX;
	for (; upa; upa--) {
		int allocs = 0, wasted = 0, nr_units = 0;

		for (group = 0; group < nr_groups; group++) {
			int this_allocs = DIV_ROUND_UP(group_cnt[group], upa);
			allocs += this_allocs;
			wasted += this_allocs * upa - group_cnt[group];
			nr_units += roundup(group_cnt[group], upa);
		}
		printf("upa: %d\n", upa);
		printf("   allocs: %d\n", allocs);
		printf("   cpus/3: %d\n", num_possible_cpus / 3);
		printf("   wasted: %d\n", wasted);
		printf("   last_allocs: %d\n", last_allocs);
		printf("   nr_units: %d\n", nr_units);

		/*
		 * Don't accept if wastage is over 1/3.  The
		 * greater-than comparison ensures upa==1 always
		 * passes the following check.
		 */
		if (wasted > num_possible_cpus / 3)
			continue;

		/* and then don't consume more memory */
		if (allocs > last_allocs)
			break;
		last_allocs = allocs;
		best_upa = upa;
		printf("   best_upa: %d\n", best_upa);
	}

	printf("Final best_upa: %d\n", best_upa);
}

void best_upa_test()
{
	int group_cnt[12];
	int i;
	int nr_groups;
	int upa;
	
	/* 
	 * Two groups
	 * Each group has 3 cpus 
	 * And the upa starts from 2
	 */
	nr_groups = 2;
	upa = 2;
	for (i = 0; i < nr_groups; i ++)
		group_cnt[i] = 3;

	printf("--- Groups: 2, upa: 2 ---\n");
	best_upa_calculation(group_cnt, nr_groups, upa);

	/* 
	 * Three groups
	 * Each group has 4 cpus 
	 * And the upa starts from 3
	 */
	nr_groups = 3;
	upa = 3;
	for (i = 0; i < nr_groups; i ++)
		group_cnt[i] = 4;

	printf("--- Groups: 3, upa: 3 ---\n");
	best_upa_calculation(group_cnt, nr_groups, upa);

	/*
	 * Three groups
	 * Each group has 4 cpus
	 * And the upa starts from 3
	 */
	nr_groups = 1;
	upa = 3;
	for (i = 0; i < nr_groups; i ++)
		group_cnt[i] = 1;

	printf("--- Groups: 3, upa: 3 ---\n");
	best_upa_calculation(group_cnt, nr_groups, upa);

}
int main()
{
	best_upa_test();

	return 0;
}
