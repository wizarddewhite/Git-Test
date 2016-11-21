#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split_mem_range.h"

static int page_size_mask;
struct map_range mr[NR_RANGE_MR];

static const char *page_size_string(struct map_range *mr)
{
	static const char str_1g[] = "1G";
	static const char str_2m[] = "2M";
	static const char str_4m[] = "4M";
	static const char str_4k[] = "4k";

	if (mr->page_size_mask & (1<<PG_LEVEL_1G))
		return str_1g;
	/*
	 * 32-bit without PAE has a 4M large page size.
	 * PG_LEVEL_2M is misnamed, but we can at least
	 * print out the right size in the string.
	 */
	if (/*  IS_ENABLED(CONFIG_X86_32) &&
	    !IS_ENABLED(CONFIG_X86_PAE) &&*/ 
	    mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_4m;

	if (mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_2m;

	return str_4k;
}

static int save_mr(struct map_range *mr, int nr_range,
			     unsigned long start_pfn, unsigned long end_pfn,
			     unsigned long page_size_mask)
{
	if (start_pfn < end_pfn) {
		if (nr_range >= NR_RANGE_MR) {
			printf("run out of range for init_memory_mapping\n");
			exit(-1);
		}
		mr[nr_range].start = start_pfn<<PAGE_SHIFT;
		mr[nr_range].end   = end_pfn<<PAGE_SHIFT;
		mr[nr_range].page_size_mask = page_size_mask;
		nr_range++;
	}

	return nr_range;
}

static int split_mem_range(struct map_range *mr, int nr_range,
				     unsigned long start,
				     unsigned long end)
{
	unsigned long start_pfn, end_pfn, limit_pfn;
	unsigned long pfn;
	int i;

	limit_pfn = PFN_DOWN(end);

	/* head if not big page alignment ? */
	pfn = start_pfn = PFN_DOWN(start);
	printf("## start    :0x%08lx, end      :0x%08lx\n", start, end);
	printf("## limit_pfn:0x%08lx\n", limit_pfn);
	printf("-- processing head --\n");
	printf("   start_pfn:0x%08lx (Page Aligned)\n", start_pfn);
#ifdef CONFIG_X86_32
	/*
	 * Don't use a large page for the first 2/4MB of memory
	 * because there are often fixed size MTRRs in there
	 * and overlapping MTRRs into large pages can cause
	 * slowdowns.
	 */
	if (pfn == 0)
		end_pfn = PFN_DOWN(PMD_SIZE);
	else
		end_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
#else /* CONFIG_X86_64 */
	end_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	printf("   end_pfn  :0x%08lx (Page Aligned)\n", end_pfn);
#endif
	if (end_pfn > limit_pfn)
		end_pfn = limit_pfn;
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
		pfn = end_pfn;
	}

	/* big page (2M) range */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	printf("-- processing 2M page --\n");
	printf("   start_pfn:0x%08lx (PMD Aligned)\n", start_pfn);
#ifdef CONFIG_X86_32
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
	printf("  end_pfn  :0x%08lx\n", end_pfn);
#else /* CONFIG_X86_64 */
	end_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	printf("   end_pfn  :0x%08lx (PUD Aligned)\n", end_pfn);
	if (end_pfn > round_down(limit_pfn, PFN_DOWN(PMD_SIZE))) {
		end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
		printf("   end_pfn  :0x%08lx\n", end_pfn);
	}
#endif

	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}

#ifdef CONFIG_X86_32
#else //CONFIG_X86_64
	/* big page (1G) range */
	start_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PUD_SIZE));
	printf("-- processing 1G page --\n");
	printf("   start_pfn:0x%08lx (PUD Aligned)\n", start_pfn);
	printf("   end_pfn  :0x%08lx (PUD Aligned)\n", end_pfn);
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask &
				 ((1<<PG_LEVEL_2M)|(1<<PG_LEVEL_1G)));
		pfn = end_pfn;
	}

	/* tail is not big page (1G) alignment */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
	printf("-- processing 2M page --\n");
	printf("   start_pfn:0x%08lx (PMD Aligned)\n", start_pfn);
	printf("   end_pfn  :0x%08lx (PMD Aligned)\n", end_pfn);
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}
#endif

	/* tail is not big page (2M) alignment */
	start_pfn = pfn;
	end_pfn = limit_pfn;
	printf("-- processing tail page --\n");
	printf("   start_pfn:0x%08lx (Page Aligned)\n", start_pfn);
	printf("   end_pfn  :0x%08lx (Page Aligned)\n", end_pfn);
	nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

	///if (!after_bootmem)
	//	adjust_range_page_size_mask(mr, nr_range);

	/* try to merge same page size and continuous */
	for (i = 0; nr_range > 1 && i < nr_range - 1; i++) {
		unsigned long old_start;
		if (mr[i].end != mr[i+1].start ||
		    mr[i].page_size_mask != mr[i+1].page_size_mask)
			continue;
		/* move it */
		old_start = mr[i].start;
		memmove(&mr[i], &mr[i+1],
			(nr_range - 1 - i) * sizeof(struct map_range));
		mr[i--].start = old_start;
		nr_range--;
	}

	for (i = 0; i < nr_range; i++)
		printf(" [mem %#010lx-%#010lx] page %s\n",
				mr[i].start, mr[i].end - 1,
				page_size_string(&mr[i]));

	return nr_range;
}

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
