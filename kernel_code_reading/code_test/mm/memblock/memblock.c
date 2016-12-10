/*
 * =====================================================================================
 *
 *       Filename:  memblock.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2015 09:00:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "memblock.h"

int memblock_debug ;
bool movable_node_enabled = false;

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

/* adjust *@size so that (@base + *@size) doesn't overflow, return new size */
static inline phys_addr_t memblock_cap_size(phys_addr_t base, phys_addr_t *size)
{
	return *size = min(*size, (phys_addr_t)ULLONG_MAX - base);
}

static void memblock_merge_regions(struct memblock_type *type)
{
	int i = 0;

	/* cnt never goes below 1 */
	while (i < type->cnt - 1) {
		struct memblock_region *this = &type->regions[i];
		struct memblock_region *next = &type->regions[i + 1];

		if (this->base + this->size != next->base ||
		    memblock_get_region_node(this) !=
		    memblock_get_region_node(next) ||
		    this->flags != next->flags) {
			if (this->base + this->size > next->base) {
				printf("%s: Error: memblock region overlap\n", __func__);
			}
			i++;
			continue;
		}

		this->size += next->size;
		/* move forward from next + 1, index of which is i + 2 */
		memmove(next, next + 1, (type->cnt - (i + 2)) * sizeof(*next));
		type->cnt--;
	}
}

static void memblock_insert_region(struct memblock_type *type,
						   int idx, phys_addr_t base,
						   phys_addr_t size,
						   int nid, unsigned long flags)
{
	struct memblock_region *rgn = &type->regions[idx];

	if (type->cnt >= type->max) {
		printf("%s: ERROR: no extra space\n", __func__);
	}
	memmove(rgn + 1, rgn, (type->cnt - idx) * sizeof(*rgn));
	rgn->base = base;
	rgn->size = size;
	rgn->flags = flags;
	memblock_set_region_node(rgn, nid);
	type->cnt++;
	type->total_size += size;
}

int memblock_add_range(struct memblock_type *type,
				phys_addr_t base, phys_addr_t size,
				int nid, unsigned long flags)
{
	bool insert = false;
	phys_addr_t obase = base;
	phys_addr_t end = base + memblock_cap_size(base, &size);
	int i, nr_new;

	if (!size)
		return 0;

	/* special case for empty array */
	if (type->regions[0].size == 0) {
		if(type->cnt != 1 || type->total_size) {
			printf("%s: cnt != 1 or has size\n", __func__);
		}
		type->regions[0].base = base;
		type->regions[0].size = size;
		type->regions[0].flags = flags;
		memblock_set_region_node(&type->regions[0], nid);
		type->total_size = size;
		return 0;
	}
repeat:
	/*
	 * The following is executed twice.  Once with %false @insert and
	 * then with %true.  The first counts the number of regions needed
	 * to accomodate the new area.  The second actually inserts them.
	 */
	base = obase;
	nr_new = 0;

	for (i = 0; i < type->cnt; i++) {
		struct memblock_region *rgn = &type->regions[i];
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;
		/*
		 * @rgn overlaps.  If it separates the lower part of new
		 * area, insert that portion.
		 */
		if (rbase > base) {
			nr_new++;
			if (insert) {
				rgn->base = base;
				rgn->size += rbase - base;
				type->total_size += rbase - base;
			}
		}
		/* area below @rend is dealt with, forget about it */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert)
			memblock_insert_region(type, i, base, end - base,
					       nid, flags);
	}

	/*
	 * If this was the first round, resize array and repeat for actual
	 * insertions; otherwise, merge and return.
	 */
	if (!insert) {
		if (type->cnt + nr_new > type->max) {
			printf("%s: Error no extra space for memblock region\n", __func__);
				return -10;
		}
		insert = true;
		goto repeat;
	} else {
		memblock_merge_regions(type);
		return 0;
	}
}

void memblock_dump(struct memblock_type *type, char *name)
{
	unsigned long long base, size;
	unsigned long flags;
	int idx;
	struct memblock_region *rgn;

	printf(" %s.cnt  = 0x%lx\n", name, type->cnt);

	for_each_memblock_type(type, rgn) {
		char nid_buf[32] = "";

		base = rgn->base;
		size = rgn->size;
		flags = rgn->flags;
		if (memblock_get_region_node(rgn) != MAX_NUMNODES)
			snprintf(nid_buf, sizeof(nid_buf), " on node %d",
				 memblock_get_region_node(rgn));
		printf(" %s[%#x]\t[%#016llx-%#016llx], %#llx bytes%s flags: %#lx\n",
			name, idx, base, base + size - 1, size, nid_buf, flags);
	}
}

void __memblock_dump_all(void)
{
	printf("MEMBLOCK configuration:\n");
	printf(" memory size = %#llx reserved size = %#llx\n",
		(unsigned long long)memblock.memory.total_size,
		(unsigned long long)memblock.reserved.total_size);

	memblock_dump(&memblock.memory, "memory");
	memblock_dump(&memblock.reserved, "reserved");
}

int memblock_search(struct memblock_type *type, phys_addr_t addr)
{
	unsigned int left = 0, right = type->cnt;

	do {
		unsigned int mid = (right + left) / 2;

		if (addr < type->regions[mid].base)
			right = mid;
		else if (addr >= (type->regions[mid].base +
				  type->regions[mid].size))
			left = mid + 1;
		else
			return mid;
	} while (left < right);
	return -1;
}

int memblock_double_array(struct memblock_type *type,
						phys_addr_t new_area_start,
						phys_addr_t new_area_size)
{
	return 0;
}

/**
 * memblock_isolate_range - isolate given range into disjoint memblocks
 * @type: memblock type to isolate range for
 * @base: base of range to isolate
 * @size: size of range to isolate
 * @start_rgn: out parameter for the start of isolated region
 * @end_rgn: out parameter for the end of isolated region
 *
 * Walk @type and ensure that regions don't cross the boundaries defined by
 * [@base,@base+@size).  Crossing regions are split at the boundaries,
 * which may create at most two more regions.  The index of the first
 * region inside the range is returned in *@start_rgn and end in *@end_rgn.
 *
 * RETURNS:
 * 0 on success, -errno on failure.
 */
int memblock_isolate_range(struct memblock_type *type,
					phys_addr_t base, phys_addr_t size,
					int *start_rgn, int *end_rgn)
{
	phys_addr_t end = base + memblock_cap_size(base, &size);
	int idx;
	struct memblock_region *rgn;

	*start_rgn = *end_rgn = 0;

	if (!size)
		return 0;

	/* we'll create at most two more regions */
	while (type->cnt + 2 > type->max)
		if (memblock_double_array(type, base, size) < 0)
			return -2;

	for_each_memblock_type(type, rgn) {
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;

		if (rbase < base) {
			/*
			 * @rgn intersects from below.  Split and continue
			 * to process the next region - the new top half.
			 */
			rgn->base = base;
			rgn->size -= base - rbase;
			type->total_size -= base - rbase;
			memblock_insert_region(type, idx, rbase, base - rbase,
					       memblock_get_region_node(rgn),
					       rgn->flags);
		} else if (rend > end) {
			/*
			 * @rgn intersects from above.  Split and redo the
			 * current region - the new bottom half.
			 */
			rgn->base = end;
			rgn->size -= end - rbase;
			type->total_size -= end - rbase;
			memblock_insert_region(type, idx--, rbase, end - rbase,
					       memblock_get_region_node(rgn),
					       rgn->flags);
		} else {
			/* @rgn is fully contained, record it */
			if (!*end_rgn)
				*start_rgn = idx;
			*end_rgn = idx + 1;
		}
	}

	return 0;
}

void memblock_remove_region(struct memblock_type *type, unsigned long r)
{
	type->total_size -= type->regions[r].size;
	memmove(&type->regions[r], &type->regions[r + 1],
		(type->cnt - (r + 1)) * sizeof(type->regions[r]));
	type->cnt--;

	/* Special case for empty arrays */
	if (type->cnt == 0) {
		//WARN_ON(type->total_size != 0);
		type->cnt = 1;
		type->regions[0].base = 0;
		type->regions[0].size = 0;
		type->regions[0].flags = 0;
		memblock_set_region_node(&type->regions[0], MAX_NUMNODES);
	}
}

int memblock_remove_range(struct memblock_type *type,
					  phys_addr_t base, phys_addr_t size)
{
	int start_rgn, end_rgn;
	int i, ret;

	ret = memblock_isolate_range(type, base, size, &start_rgn, &end_rgn);
	if (ret)
		return ret;

	for (i = end_rgn - 1; i >= start_rgn; i--)
		memblock_remove_region(type, i);
	return 0;
}

int memblock_reserve(phys_addr_t base, phys_addr_t size)
{
	//memblock_dbg("memblock_reserve: [%#016llx-%#016llx] flags %#02lx %pF\n",
	//	     (unsigned long long)base,
	//	     (unsigned long long)base + size - 1,
	//	     0UL, (void *)_RET_IP_);

	return memblock_add_range(&memblock.reserved, base, size, MAX_NUMNODES, 0);
}

static phys_addr_t memblock_alloc_range_nid(phys_addr_t size,
					phys_addr_t align, phys_addr_t start,
					phys_addr_t end, int nid, unsigned long flags)
{
	phys_addr_t found;

	if (!align)
		align = 8;

	found = memblock_find_in_range_node(size, align, start, end, nid,
					    flags);
	if (found && !memblock_reserve(found, size)) {
		/*
		 * The min_count is set to 0 so that memblock allocations are
		 * never reported as leaks.
		 */
		//kmemleak_alloc(__va(found), size, 0, 0);
		return found;
	}
	return 0;
}

static phys_addr_t memblock_alloc_base_nid(phys_addr_t size,
					phys_addr_t align, phys_addr_t max_addr,
					int nid, unsigned long flags)
{
	return memblock_alloc_range_nid(size, align, 0, max_addr, nid, flags);
}

phys_addr_t __memblock_alloc_base(phys_addr_t size, phys_addr_t align, phys_addr_t max_addr)
{
	return memblock_alloc_base_nid(size, align, max_addr, NUMA_NO_NODE,
				       MEMBLOCK_NONE);
}

phys_addr_t memblock_alloc_base(phys_addr_t size, phys_addr_t align, phys_addr_t max_addr)
{
	phys_addr_t alloc;

	alloc = __memblock_alloc_base(size, align, max_addr);

	if (alloc == 0) {
		printf("ERROR: Failed to allocate 0x%llx bytes below 0x%llx.\n",
		      (unsigned long long) size, (unsigned long long) max_addr);
		exit(1);
	}

	return alloc;
}

phys_addr_t memblock_alloc(phys_addr_t size, phys_addr_t align)
{
	return memblock_alloc_base(size, align, MEMBLOCK_ALLOC_ACCESSIBLE);
}

/**
 * __memblock_find_range_top_down - find free area utility, in top-down
 * @start: start of candidate range
 * @end: end of candidate range, can be %MEMBLOCK_ALLOC_{ANYWHERE|ACCESSIBLE}
 * @size: size of free area to find
 * @align: alignment of free area to find
 * @nid: nid of the free area to find, %NUMA_NO_NODE for any node
 * @flags: pick from blocks based on memory attributes
 *
 * Utility called from memblock_find_in_range_node(), find free area top-down.
 *
 * RETURNS:
 * Found address on success, 0 on failure.
 */

#define clamp(val, lo, hi) min_t(max_t(val, lo), hi)
#define __round_mask(x, y) ((__typeof__(x))((y)-1))
#define round_up(x, y) ((((x)-1) | __round_mask(x, y))+1)
#define round_down(x, y) ((x) & ~__round_mask(x, y))

static phys_addr_t 
__memblock_find_range_top_down(phys_addr_t start, phys_addr_t end,
			       phys_addr_t size, phys_addr_t align, int nid,
			       unsigned long flags)
{
	phys_addr_t this_start, this_end, cand;
	u64 i;

	for_each_free_mem_range_reverse(i, nid, flags, &this_start, &this_end,
					NULL) {
		this_start = clamp(this_start, start, end);
		this_end = clamp(this_end, start, end);

		if (this_end < size)
			continue;

		cand = round_down(this_end - size, align);
		if (cand >= this_start)
			return cand;
	}

	return 0;
}

/**
 * memblock_find_in_range_node - find free area in given range and node
 * @size: size of free area to find
 * @align: alignment of free area to find
 * @start: start of candidate range
 * @end: end of candidate range, can be %MEMBLOCK_ALLOC_{ANYWHERE|ACCESSIBLE}
 * @nid: nid of the free area to find, %NUMA_NO_NODE for any node
 * @flags: pick from blocks based on memory attributes
 *
 * Find @size free area aligned to @align in the specified range and node.
 *
 * When allocation direction is bottom-up, the @start should be greater
 * than the end of the kernel image. Otherwise, it will be trimmed. The
 * reason is that we want the bottom-up allocation just near the kernel
 * image so it is highly likely that the allocated memory and the kernel
 * will reside in the same node.
 *
 * If bottom-up allocation failed, will try to allocate memory top-down.
 *
 * RETURNS:
 * Found address on success, 0 on failure.
 */
phys_addr_t memblock_find_in_range_node(phys_addr_t size,
					phys_addr_t align, phys_addr_t start,
					phys_addr_t end, int nid, ulong flags)
{
	phys_addr_t kernel_end, ret;

	/* pump up @end */
	if (end == MEMBLOCK_ALLOC_ACCESSIBLE)
		end = memblock.current_limit;

	/* avoid allocating the first page */
	start = max_t(start, 4096/* PAGE_SIZE */);
	end = max_t(start, end);
	kernel_end = 0xf0000000; //__pa_symbol(_end);

	/*
	 * try bottom-up allocation only when bottom-up mode
	 * is set and @end is above the kernel image.
	 */
#if 0
	if (memblock_bottom_up() && end > kernel_end) {
		phys_addr_t bottom_up_start;

		/* make sure we will allocate above the kernel */
		bottom_up_start = max(start, kernel_end);

		/* ok, try bottom-up allocation first */
		ret = __memblock_find_range_bottom_up(bottom_up_start, end,
						      size, align, nid, flags);
		if (ret)
			return ret;

		/*
		 * we always limit bottom-up allocation above the kernel,
		 * but top-down allocation doesn't have the limit, so
		 * retrying top-down allocation may succeed when bottom-up
		 * allocation failed.
		 *
		 * bottom-up allocation is expected to be fail very rarely,
		 * so we use WARN_ONCE() here to see the stack trace if
		 * fail happens.
		 */
		WARN_ONCE(1, "memblock: bottom-up allocation failed, memory hotunplug may be affected\n");
	}
#endif

	return __memblock_find_range_top_down(start, end, size, align, nid,
					      flags);
}

/**
 * __next_mem_range_rev - generic next function for for_each_*_range_rev()
 *
 * Finds the next range from type_a which is not marked as unsuitable
 * in type_b.
 *
 * @idx: pointer to u64 loop variable
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @type_a: pointer to memblock_type from where the range is taken
 * @type_b: pointer to memblock_type which excludes memory from being taken
 * @out_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @out_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @out_nid: ptr to int for nid of the range, can be %NULL
 *
 * Reverse of __next_mem_range().
 */
void __next_mem_range_rev(u64 *idx, int nid, ulong flags,
					  struct memblock_type *type_a,
					  struct memblock_type *type_b,
					  phys_addr_t *out_start,
					  phys_addr_t *out_end, int *out_nid)
{
	int idx_a = *idx & 0xffffffff;
	int idx_b = *idx >> 32;

	/* 
	if (WARN_ONCE(nid == MAX_NUMNODES, "Usage of MAX_NUMNODES is deprecated. Use NUMA_NO_NODE instead\n"))
		nid = NUMA_NO_NODE;
	 */
	if (*idx == (u64)ULLONG_MAX) {
		idx_a = type_a->cnt - 1;
		idx_b = type_b->cnt;
	}

	for (; idx_a >= 0; idx_a--) {
		struct memblock_region *m = &type_a->regions[idx_a];

		phys_addr_t m_start = m->base;
		phys_addr_t m_end = m->base + m->size;
		int m_nid = memblock_get_region_node(m);

		/* only memory regions are associated with nodes, check it */
		if (nid != NUMA_NO_NODE && nid != m_nid)
			continue;

		/* skip hotpluggable memory regions if needed */
		if (movable_node_is_enabled() && memblock_is_hotpluggable(m))
			continue;

		/* if we want mirror memory skip non-mirror memory regions */
		if ((flags & MEMBLOCK_MIRROR) && !memblock_is_mirror(m))
			continue;

		/* skip nomap memory unless we were asked for it explicitly */
		if (!(flags & MEMBLOCK_NOMAP) && memblock_is_nomap(m))
			continue;

		if (!type_b) {
			if (out_start)
				*out_start = m_start;
			if (out_end)
				*out_end = m_end;
			if (out_nid)
				*out_nid = m_nid;
			idx_a++;
			*idx = (u32)idx_a | (u64)idx_b << 32;
			return;
		}

		/* scan areas before each reservation */
		for (; idx_b >= 0; idx_b--) {
			struct memblock_region *r;
			phys_addr_t r_start;
			phys_addr_t r_end;

			r = &type_b->regions[idx_b];
			r_start = idx_b ? r[-1].base + r[-1].size : 0;
			r_end = idx_b < type_b->cnt ?
				r->base : ULLONG_MAX;
			/*
			 * if idx_b advanced past idx_a,
			 * break out to advance idx_a
			 */

			if (r_end <= m_start)
				break;
			/* if the two regions intersect, we're done */
			if (m_end > r_start) {
				if (out_start)
					*out_start = max_t(m_start, r_start);
				if (out_end)
					*out_end = min_t(m_end, r_end);
				if (out_nid)
					*out_nid = m_nid;
				if (m_start >= r_start)
					idx_a--;
				else
					idx_b--;
				*idx = (u32)idx_a | (u64)idx_b << 32;
				return;
			}
		}
	}
	/* signal end of iteration */
	*idx = ULLONG_MAX;
}


