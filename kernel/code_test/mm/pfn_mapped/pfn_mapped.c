#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "range.h"

#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)

struct range pfn_mapped[E820_X_MAX];
int nr_pfn_mapped;

int add_range(struct range *range, int az, int nr_range, u64 start, u64 end)
{
	if (start >= end)
		return nr_range;

	/* Out of slots: */
	if (nr_range >= az)
		return nr_range;

	range[nr_range].start = start;
	range[nr_range].end = end;

	nr_range++;

	return nr_range;
}

int add_range_with_merge(struct range *range, int az, int nr_range,
		     u64 start, u64 end)
{
	int i;

	if (start >= end)
		return nr_range;

	/* get new start/end: */
	for (i = 0; i < nr_range; i++) {
		u64 common_start, common_end;

		if (!range[i].end)
			continue;

		common_start = max(range[i].start, start);
		common_end = min(range[i].end, end);
		if (common_start > common_end)
			continue;

		/* new start/end, will add it back at last */
		start = min(range[i].start, start);
		end = max(range[i].end, end);

		memmove(&range[i], &range[i + 1],
			(nr_range - (i + 1)) * sizeof(range[i]));
		range[nr_range - 1].start = 0;
		range[nr_range - 1].end   = 0;
		nr_range--;
		i--;
	}

	/* Need to add it: */
	return add_range(range, az, nr_range, start, end);
}

static int cmp_range(const void *x1, const void *x2)
{
	const struct range *r1 = x1;
	const struct range *r2 = x2;

	if (r1->start < r2->start)
		return -1;
	if (r1->start > r2->start)
		return 1;
	return 0;
}

int clean_sort_range(struct range *range, int az)
{
	int i, j, k = az - 1, nr_range = az;

	for (i = 0; i < k; i++) {
		if (range[i].end)
			continue;
		for (j = k; j > i; j--) {
			if (range[j].end) {
				k = j;
				break;
			}
		}
		if (j == i)
			break;
		range[i].start = range[k].start;
		range[i].end   = range[k].end;
		range[k].start = 0;
		range[k].end   = 0;
		k--;
	}
	/* count it */
	for (i = 0; i < az; i++) {
		if (!range[i].end) {
			nr_range = i;
			break;
		}
	}

	/* sort them */
	qsort(range, nr_range, sizeof(struct range), cmp_range);

	return nr_range;
}

void dump_pfn_mapped()
{
	int i;

	for (i = 0; i < nr_pfn_mapped; i++) {
		printf("[%2d] 0x%016llx - 0x%016llx\n", i, pfn_mapped[i].start, pfn_mapped[i].end);
	}
}
