#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SZ_4K				0x00001000UL
#define SZ_16K				0x00004000UL
#define SZ_2M				0x00200000UL
#define SZ_4M				0x00400000UL
#define SZ_64M				0x04000000UL
#define SZ_1G				0x40000000UL
#define SZ_2G				0x80000000UL

#define CONFIG_X86_64 1
//#define CONFIG_X86_32 0
#define NR_RANGE_MR 5

#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

#define PMD_SHIFT	21
#define PMD_SIZE	(_AC(1, UL) << PMD_SHIFT)
#define PUD_SHIFT	30
#define PUD_SIZE	(_AC(1, UL) << PUD_SHIFT)

#define PFN_ALIGN(x)	(((unsigned long)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)
#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x)	((unsigned long)((x) >> PAGE_SHIFT))

#define IS_ALIGNED(x, a)		(((x) & ((typeof(x))(a) - 1)) == 0)
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define max(a,b) 		\
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) 		\
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

enum pg_level {
	PG_LEVEL_NONE,
	PG_LEVEL_4K,
	PG_LEVEL_2M,
	PG_LEVEL_1G,
	PG_LEVEL_512G,
	PG_LEVEL_NUM
};

static int page_size_mask = (1 << PG_LEVEL_2M) | (1 << PG_LEVEL_1G);
struct map_range {
	unsigned long start;
	unsigned long end;
	unsigned page_size_mask;
};

struct mapinfo {
	unsigned int	mask;
	unsigned int	size;
};

struct map_range range[NR_RANGE_MR];

static int save_mr(struct map_range *mr, int nr_range,
			     unsigned long start_pfn, unsigned long end_pfn,
			     unsigned long page_size_mask)
{
	if (start_pfn < end_pfn) {
		if (nr_range >= NR_RANGE_MR)
			//panic("run out of range for init_memory_mapping\n");
			exit(-1);
		mr[nr_range].start = start_pfn<<PAGE_SHIFT;
		mr[nr_range].end   = end_pfn<<PAGE_SHIFT;
		mr[nr_range].page_size_mask = page_size_mask;
		nr_range++;
	}

	return nr_range;
}

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
	//if (IS_ENABLED(CONFIG_X86_32) &&
	 //   !IS_ENABLED(CONFIG_X86_PAE) &&
	  //  mr->page_size_mask & (1<<PG_LEVEL_2M))
	//	return str_4m;

	if (mr->page_size_mask & (1<<PG_LEVEL_2M))
		return str_2m;

	return str_4k;
}

bool memblock_is_region_memory(unsigned long base, unsigned long size)
{
#if 0
	int idx = memblock_search(&memblock.memory, base);
	phys_addr_t end = base + memblock_cap_size(base, &size);

	if (idx == -1)
		return false;
	return (memblock.memory.regions[idx].base +
		 memblock.memory.regions[idx].size) >= end;
#endif

	if (base + size >= SZ_2G || base < SZ_4K)
		return false;

	return true;
}

static void adjust_range_page_size_mask(struct map_range *mr,
					 int nr_range)
{
	int i;

	for (i = 0; i < nr_range; i++) {
		if ((page_size_mask & (1<<PG_LEVEL_2M)) &&
		    !(mr[i].page_size_mask & (1<<PG_LEVEL_2M))) {
			unsigned long start = round_down(mr[i].start, PMD_SIZE);
			unsigned long end = round_up(mr[i].end, PMD_SIZE);

#ifdef CONFIG_X86_32
			if ((end >> PAGE_SHIFT) > max_low_pfn)
				continue;
#endif

			if (memblock_is_region_memory(start, end - start))
				mr[i].page_size_mask |= 1<<PG_LEVEL_2M;
		}
		if ((page_size_mask & (1<<PG_LEVEL_1G)) &&
		    !(mr[i].page_size_mask & (1<<PG_LEVEL_1G))) {
			unsigned long start = round_down(mr[i].start, PUD_SIZE);
			unsigned long end = round_up(mr[i].end, PUD_SIZE);

			if (memblock_is_region_memory(start, end - start))
				mr[i].page_size_mask |= 1<<PG_LEVEL_1G;
		}
	}
}
static int split_mem_range(struct map_range *mr, int nr_range,
			     unsigned long start, unsigned long end,
			     bool after_bootmem)
{
	unsigned long start_pfn, end_pfn, limit_pfn;
	unsigned long pfn;
	int i;

	limit_pfn = PFN_DOWN(end);

	/* head if not big page alignment ? */
	pfn = start_pfn = PFN_DOWN(start);
#ifndef CONFIG_X86_64
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
#endif
	if (end_pfn > limit_pfn)
		end_pfn = limit_pfn;
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
		pfn = end_pfn;
	}

	/* big page (2M) range */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
#ifndef CONFIG_X86_64
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
#else /* CONFIG_X86_64 */
	end_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	if (end_pfn > round_down(limit_pfn, PFN_DOWN(PMD_SIZE)))
		end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
#endif

	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}

#ifdef CONFIG_X86_64
	/* big page (1G) range */
	start_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PUD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask &
				 ((1<<PG_LEVEL_2M)|(1<<PG_LEVEL_1G)));
		pfn = end_pfn;
	}

	/* tail is not big page (1G) alignment */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}
#endif

	/* tail is not big page (2M) alignment */
	start_pfn = pfn;
	end_pfn = limit_pfn;
	nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);

	if (!after_bootmem) {
		adjust_range_page_size_mask(mr, nr_range);
		printf("Memory Region after adjust\n");
		for (i = 0; i < nr_range; i++)
			printf(" [mem %#010lx-%#010lx] page %s mask %x\n",
				mr[i].start, mr[i].end - 1,
				page_size_string(&mr[i]),
				mr[i].page_size_mask);
	}

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

	printf("Memory Region after merge\n");
	for (i = 0; i < nr_range; i++)
		printf(" [mem %#010lx-%#010lx] page %s mask %x\n",
				mr[i].start, mr[i].end - 1,
				page_size_string(&mr[i]),
				mr[i].page_size_mask);

	return nr_range;
}

static int split_mem_range2(struct map_range *mr, int nr_range,
			     unsigned long start, unsigned long end,
			     bool after_bootmem)
{
	unsigned long start_pfn, end_pfn, limit_pfn;
	unsigned long pfn;
	int i;

	limit_pfn = PFN_DOWN(end);

	/* head if not big page alignment ? */
	pfn = start_pfn = PFN_DOWN(start);
#ifndef CONFIG_X86_64
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
#endif
	if (end_pfn > limit_pfn)
		goto only_4k_range;
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);
		pfn = end_pfn;
	}

#ifdef CONFIG_X86_64
	/* big page (2M) range */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	end_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	if (end_pfn > limit_pfn)
		goto no_1g_range;

	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}

	/* big page (1G) range */
	start_pfn = round_up(pfn, PFN_DOWN(PUD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PUD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask &
				 ((1<<PG_LEVEL_2M)|(1<<PG_LEVEL_1G)));
		pfn = end_pfn;
	}
#endif

no_1g_range:
	/* big page (2M) range */
	start_pfn = round_up(pfn, PFN_DOWN(PMD_SIZE));
	end_pfn = round_down(limit_pfn, PFN_DOWN(PMD_SIZE));
	if (start_pfn < end_pfn) {
		nr_range = save_mr(mr, nr_range, start_pfn, end_pfn,
				page_size_mask & (1<<PG_LEVEL_2M));
		pfn = end_pfn;
	}

only_4k_range:
	/* tail is not big page (2M) alignment */
	start_pfn = pfn;
	end_pfn = limit_pfn;
	nr_range = save_mr(mr, nr_range, start_pfn, end_pfn, 0);


	if (!after_bootmem)
		adjust_range_page_size_mask(mr, nr_range);

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

static void mr_print(struct map_range *mr, unsigned int maxidx)
{
#if defined(CONFIG_X86_32) && !defined(CONFIG_X86_PAE)
	static const char *sz[2] = { "4K", "4M" };
#else
	static const char *sz[4] = { "4K", "2M", "1G", "" };
#endif
	unsigned int idx, s;

	for (idx = 0; idx < maxidx; idx++, mr++) {
		s = (mr->page_size_mask >> PG_LEVEL_2M) & (ARRAY_SIZE(sz) -1);
		printf("[mem %#010lx-%#010lx] page size %s\n",
			 mr->start, mr->end - 1, sz[s]);
 	}
}
static void mr_expand(struct map_range *mr, unsigned int size)
{
	unsigned long start = round_down(mr->start, size);
	unsigned long end = round_up(mr->end, size);

#ifdef CONFIG_X86_32
	if ((end >> PAGE_SHIFT) > max_low_pfn)
		return;
#endif

	if (memblock_is_region_memory(start, end - start)) {
		mr->start = start;
		mr->end = end;
	}
}

static bool mr_try_map(struct map_range *mr, const struct mapinfo *mi,
		bool after_bootmem)
{
	unsigned long len;

	/* Check whether the map size is supported. PAGE_SIZE always is. */
	if (mi->mask && !(mr->page_size_mask & mi->mask))
		return false;

	if (!after_bootmem) {
		//printf("%s: -> [mem %#010lx-%#010lx] \n",
		//	__func__, mr->start, mr->end - 1);
		mr_expand(mr, mi->size);
		//printf("%s: <- [mem %#010lx-%#010lx] \n",
		//	__func__, mr->start, mr->end - 1);
	}


	if (!IS_ALIGNED(mr->start, mi->size)) {
		/* Limit the range to the next boundary of this size. */
		mr->end = min(mr->end,
				round_up(mr->start, mi->size));
		return false;
	}

	if (!IS_ALIGNED(mr->end, mi->size)) {
		/* Try to fit as much as possible */
		len = round_down(mr->end - mr->start, mi->size);
		if (!len)
			return false;
		mr->end = mr->start + len;
	}

	/* Store the effective page size mask */
	mr->page_size_mask = mi->mask;
	return true;
}

static void mr_setup(struct map_range *mr, unsigned long start,
		       unsigned long end)
{
	/*
	 * On 32bit the first 2/4MB are often covered by fixed size MTRRs.
	 * Overlapping MTRRs on large pages can cause slowdowns. Force 4k
	 * mappings.
	 */
#ifdef CONFIG_X86_32
	if (start < PMD_SIZE) {
		mr->page_size_mask = 0;
		mr->end = min(end, PMD_SIZE);
	} else {
#endif
		/* Set the possible mapping sizes and allow full range. */
		mr->page_size_mask = page_size_mask;
		mr->end = end;
#ifdef CONFIG_X86_32
	}
#endif
	mr->start = start;
}

int split_mem_range_tglx(struct map_range *mr, unsigned long start,
			     unsigned long end, bool after_bootmem)
{
	static const struct mapinfo mapinfos[] = {
#ifdef CONFIG_X86_64
		{ .mask = 1U << PG_LEVEL_1G, .size = PUD_SIZE },
#endif
		{ .mask = 1U << PG_LEVEL_2M, .size = PMD_SIZE },
		{ .mask = 0, .size = PAGE_SIZE },
	};
	const struct mapinfo *mi;
	struct map_range *curmr;
	unsigned long addr;
	int idx;

	for (idx = 0, addr = start, curmr = mr; addr < end; idx++, curmr++) {
		//BUG_ON(idx == NR_RANGE_MR);

		mr_setup(curmr, addr, end);

		/* Try map sizes top down. PAGE_SIZE will always succeed. */
		for (mi = mapinfos; !mr_try_map(curmr, mi, after_bootmem); mi++);

		/* Get the start address for the next range */
		addr = curmr->end;
	}

	mr_print(mr, idx);
	return idx;
}
void split_test()
{
#ifdef CONFIG_X86_64
	printf("Original version [4k, 2G + 64M]:\n");
	split_mem_range(range, 0, SZ_4K, SZ_2G + SZ_64M, true);
	printf("Refined version [4k, 2G + 64M]:\n");
	split_mem_range2(range, 0, SZ_4K, SZ_2G + SZ_64M, true);

	printf("Original version [4k, 64M]:\n");
	split_mem_range(range, 0, SZ_4K, SZ_64M, true);
	printf("Refined version [4k, 64M]:\n");
	split_mem_range2(range, 0, SZ_4K, SZ_64M, true);

	printf("Original version [4k, 16k]:\n");
	split_mem_range(range, 0, SZ_4K, SZ_16K, true);
	printf("Refined version [4k, 16k]:\n");
	split_mem_range2(range, 0, SZ_4K, SZ_16K, true);
#else
	printf("Original version:\n");
	split_mem_range(range, 0, 0, SZ_2G, true);
	printf("Refined version:\n");
	split_mem_range2(range, 0, 0, SZ_2G, true);
#endif
}

void split_before_boot()
{
	printf("Split after boot [4k, 2G + 64M]:\n");
	split_mem_range(range, 0, SZ_4K, SZ_2G + SZ_64M, true);
	printf("Split before boot [4k, 2G + 64M]:\n");
	split_mem_range(range, 0, SZ_4K, SZ_2G + SZ_64M, false);
}

void split_before_boot_tglx()
{
	printf("### Split after boot       [4k, 2G][%#010lx-%#010lx]:\n",
			SZ_4K, SZ_2G);
	split_mem_range(range, 0, SZ_4K, SZ_2G, true);
	printf("### Split before boot      [4k, 2G][%#010lx-%#010lx]:\n",
			SZ_4K, SZ_2G);
	split_mem_range(range, 0, SZ_4K, SZ_2G, false);
	printf("### Split before boot tglx [4k, 2G][%#010lx-%#010lx]:\n",
			SZ_4K, SZ_2G);
	split_mem_range_tglx(range, SZ_4K, SZ_2G, false);
}

void split_boot_tglx()
{
	/* Just has 4K page */
	printf("### Split after boot tglx [4K, 16K][%#010lx-%#010lx]:\n",
			SZ_4K, SZ_16K);
	split_mem_range_tglx(range, SZ_4K, SZ_16K, true);
	/* Just has 2M page */
	printf("### Split after boot tglx [4M, 64M][%#010lx-%#010lx]:\n",
			SZ_4M, SZ_64M);
	split_mem_range_tglx(range, SZ_4M, SZ_64M, true);
	/* Just has 1G page */
	printf("### Split after boot tglx [0G, 2G][%#010lx-%#010lx]:\n",
			0UL, SZ_2G);
	split_mem_range_tglx(range, 0, SZ_2G, true);
	/* Just has 4K and 2M page */
	printf("### Split after boot tglx [16K, 4M + 16K][%#010lx-%#010lx]:\n",
			SZ_16K, SZ_4M + SZ_16K);
	split_mem_range_tglx(range, SZ_16K, SZ_4M + SZ_16K, true);
	/* Just has 2M and 1G page */
	printf("### Split after boot tglx [4M, 2G + 2M][%#010lx-%#010lx]:\n",
			SZ_4M, SZ_2G + SZ_2M);
	split_mem_range_tglx(range, SZ_4M, SZ_2G + SZ_2M, true);
	/* Has 4K, 2M and 1G page */
	printf("### Split after boot tglx [4M - 16K, 2G + 2M + 16K][%#010lx-%#010lx]:\n",
			SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K);
	split_mem_range_tglx(range, SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K, true);
	/* Has 4K, 2M and 1G page, but page_size_mask doesn't support 1G */
	page_size_mask = (1 << PG_LEVEL_2M);
	printf("### Split after boot tglx [4M - 16K, 2G + 2M + 16K][%#010lx-%#010lx]:\n",
			SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K);
	split_mem_range_tglx(range, SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K, true);
	/* Has 4K, 2M and 1G page, but page_size_mask just support 4K */
	page_size_mask = 0;
	printf("### Split after boot tglx [4M - 16K, 2G + 2M + 16K][%#010lx-%#010lx]:\n",
			SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K);
	split_mem_range_tglx(range, SZ_4M - SZ_16K, SZ_2G + SZ_2M + SZ_16K, true);
}

int main()
{
	split_boot_tglx();
	return 0;
}
