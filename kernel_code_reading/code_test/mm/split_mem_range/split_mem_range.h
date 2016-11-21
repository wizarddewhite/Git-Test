#ifndef SPLIT_MEM_RANGE
#define SPLIT_MEM_RANGE

#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)

#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)

#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

#define PUD_SHIFT	30
#define PUD_SIZE	(1UL << PUD_SHIFT)

#define PMD_SHIFT	21
#define PMD_SIZE	(1UL << PMD_SHIFT)

enum pg_level {
	PG_LEVEL_NONE,
	PG_LEVEL_4K,
	PG_LEVEL_2M,
	PG_LEVEL_1G,
	PG_LEVEL_NUM
};

#define NR_RANGE_MR 5
struct map_range {
	unsigned long start;
	unsigned long end;
	unsigned page_size_mask;
};

#endif //SPLIT_MEM_RANGE
