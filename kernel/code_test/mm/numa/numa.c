#include "numa.h"

struct numa_meminfo numa_meminfo;

void dump_meminfo(struct numa_meminfo *mi)
{
	int i;

	printf("\n---Dump meminfo\n");
	for (i = 0; i < mi->nr_blks; i++) {
		printf("NUMA: node %d [mem %#010llx-%#010llx]\n",
			mi->blk[i].nid, mi->blk[i].start,
			mi->blk[i].end - 1);
	}
}

static int numa_add_memblk_to(int nid, u64 start, u64 end,
				     struct numa_meminfo *mi)
{
	/* ignore zero length blks */
	if (start == end)
		return 0;

	/* whine about and ignore invalid blks */
	if (start > end || nid < 0 || nid >= MAX_NUMNODES) {
		printf("NUMA: Warning: invalid memblk node %d [mem %#010llx-%#010llx]\n",
			   nid, start, end - 1);
		return 0;
	}

	if (mi->nr_blks >= NR_NODE_MEMBLKS) {
		printf("NUMA: too many memblk ranges\n");
		return -22;
	}

	mi->blk[mi->nr_blks].start = start;
	mi->blk[mi->nr_blks].end = end;
	mi->blk[mi->nr_blks].nid = nid;
	mi->nr_blks++;
	return 0;
}

/**
 * numa_add_memblk - Add one numa_memblk to numa_meminfo
 * @nid: NUMA node ID of the new memblk
 * @start: Start address of the new memblk
 * @end: End address of the new memblk
 *
 * Add a new memblk to the default numa_meminfo.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int numa_add_memblk(int nid, u64 start, u64 end)
{
	return numa_add_memblk_to(nid, start, end, &numa_meminfo);
}

/**
 * numa_remove_memblk_from - Remove one numa_memblk from a numa_meminfo
 * @idx: Index of memblk to remove
 * @mi: numa_meminfo to remove memblk from
 *
 * Remove @idx'th numa_memblk from @mi by shifting @mi->blk[] and
 * decrementing @mi->nr_blks.
 */
void numa_remove_memblk_from(int idx, struct numa_meminfo *mi)
{
	mi->nr_blks--;
	memmove(&mi->blk[idx], &mi->blk[idx + 1],
		(mi->nr_blks - idx) * sizeof(mi->blk[0]));
}


/**
 * numa_cleanup_meminfo - Cleanup a numa_meminfo
 * @mi: numa_meminfo to clean up
 *
 * Sanitize @mi by merging and removing unncessary memblks.  Also check for
 * conflicts and clear unused memblks.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int numa_cleanup_meminfo(struct numa_meminfo *mi)
{
	const u64 low = 0;
	const u64 high = PFN_PHYS(ULLONG_MAX);
	int i, j, k;

	/* first, trim all entries */
	for (i = 0; i < mi->nr_blks; i++) {
		struct numa_memblk *bi = &mi->blk[i];

		/* make sure all blocks are inside the limits */
		bi->start = max(bi->start, low);
		bi->end = min(bi->end, high);

		/* and there's no empty or non-exist block */
		if (bi->start >= bi->end  /*  ||
		    !memblock_overlaps_region(&memblock.memory,
			bi->start, bi->end - bi->start)*/)
			numa_remove_memblk_from(i--, mi);
	}

	/* merge neighboring / overlapping entries */
	for (i = 0; i < mi->nr_blks; i++) {
		struct numa_memblk *bi = &mi->blk[i];

		for (j = i + 1; j < mi->nr_blks; j++) {
			struct numa_memblk *bj = &mi->blk[j];
			u64 start, end;

			/*
			 * See whether there are overlapping blocks.  Whine
			 * about but allow overlaps of the same nid.  They
			 * will be merged below.
			 */
			if (bi->end > bj->start && bi->start < bj->end) {
				if (bi->nid != bj->nid) {
					printf("NUMA: node %d [mem %#010llx-%#010llx] overlaps with node %d [mem %#010llx-%#010llx]\n",
					       bi->nid, bi->start, bi->end - 1,
					       bj->nid, bj->start, bj->end - 1);
					return -22;
				}
				printf("NUMA: Warning: node %d [mem %#010llx-%#010llx] overlaps with itself [mem %#010llx-%#010llx]\n",
					   bi->nid, bi->start, bi->end - 1,
					   bj->start, bj->end - 1);
			}

			/*
			 * Join together blocks on the same node, holes
			 * between which don't overlap with memory on other
			 * nodes.
			 */
			if (bi->nid != bj->nid)
				continue;
			start = min(bi->start, bj->start);
			end = max(bi->end, bj->end);
			for (k = 0; k < mi->nr_blks; k++) {
				struct numa_memblk *bk = &mi->blk[k];

				if (bi->nid == bk->nid)
					continue;
				if (start < bk->end && end > bk->start)
					break;
			}
			if (k < mi->nr_blks)
				continue;
			printf("NUMA: Node %d [mem %#010llx-%#010llx] + [mem %#010llx-%#010llx] -> [mem %#010llx-%#010llx]\n",
			       bi->nid, bi->start, bi->end - 1, bj->start,
			       bj->end - 1, start, end - 1);
			bi->start = start;
			bi->end = end;
			numa_remove_memblk_from(j--, mi);
		}
	}

	/* clear unused ones */
	for (i = mi->nr_blks; i < ARRAY_SIZE(mi->blk); i++) {
		mi->blk[i].start = mi->blk[i].end = 0;
		mi->blk[i].nid = NUMA_NO_NODE;
	}

	return 0;
}
