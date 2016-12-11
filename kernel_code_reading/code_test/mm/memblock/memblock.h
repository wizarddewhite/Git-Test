/*
 * =====================================================================================
 *
 *       Filename:  memblock.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2015 09:02:08 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _LINUX_MEMBLOCK_H
#define _LINUX_MEMBLOCK_H

#include <stdbool.h>

#define max_t(a,b) ((a) > (b) ? a : b)
#define min_t(a,b) ((a) < (b) ? a : b)

#define ULLONG_MAX	(~0ULL)
#define MAX_NUMNODES    (1 << 3)
#define	NUMA_NO_NODE	(-1)

typedef unsigned long phys_addr_t ;
typedef unsigned long long u64 ;
typedef unsigned int u32 ;

/* Definition of memblock flags. */
enum {
	MEMBLOCK_NONE		= 0x0,	/* No special request */
	MEMBLOCK_HOTPLUG	= 0x1,	/* hotpluggable region */
	MEMBLOCK_MIRROR		= 0x2,	/* mirrored region */
	MEMBLOCK_NOMAP		= 0x4,	/* don't add to kernel direct mapping */
};

struct memblock_region {
	phys_addr_t base;
	phys_addr_t size;
	unsigned long flags;
	int nid;
};

struct memblock_type {
	unsigned long cnt;	/* number of regions */
	unsigned long max;	/* size of the allocated array */
	phys_addr_t total_size;	/* size of all regions */
	struct memblock_region *regions;
};

struct memblock {
	bool bottom_up;  /* is bottom up direction? */
	phys_addr_t current_limit;
	struct memblock_type memory;
	struct memblock_type reserved;
#ifdef CONFIG_HAVE_MEMBLOCK_PHYS_MAP
	struct memblock_type physmem;
#endif
};

#define for_each_memblock_type(memblock_type, rgn)			\
	for (idx = 0, rgn = &memblock_type->regions[idx]; 		\
	     idx < memblock_type->cnt;					\
	     idx++,rgn = &memblock_type->regions[idx])

static phys_addr_t min(phys_addr_t a, phys_addr_t b)
{
	return (a > b)? b : a;
}

static inline void memblock_set_region_node(struct memblock_region *r, int nid)
{
	r->nid = nid;
}

static inline int memblock_get_region_node(const struct memblock_region *r)
{
	return r->nid;
}

static inline bool memblock_is_hotpluggable(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_HOTPLUG;
}

extern bool movable_node_enabled;
static inline bool movable_node_is_enabled(void)
{
	return movable_node_enabled;
}

static inline bool memblock_is_mirror(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_MIRROR;
}

static inline bool memblock_is_nomap(struct memblock_region *m)
{
	return m->flags & MEMBLOCK_NOMAP;
}

phys_addr_t memblock_find_in_range_node(phys_addr_t size, phys_addr_t align,
					    phys_addr_t start, phys_addr_t end,
					    int nid, unsigned long flags);
int memblock_add_range(struct memblock_type *type,
				phys_addr_t base, phys_addr_t size,
				int nid, unsigned long flags);
void memblock_dump(struct memblock_type *type, char *name);
int memblock_search(struct memblock_type *type, phys_addr_t addr);
int memblock_remove_range(struct memblock_type *type,
					  phys_addr_t base, phys_addr_t size);
int memblock_reserve(phys_addr_t base, phys_addr_t size);

void __next_mem_range_rev(u64 *idx, int nid, unsigned long flags,
			  struct memblock_type *type_a,
			  struct memblock_type *type_b, phys_addr_t *out_start,
			  phys_addr_t *out_end, int *out_nid);

extern struct memblock memblock;
extern int memblock_debug;
extern void __memblock_dump_all(void);
static inline void memblock_dump_all(void)
{
	if (memblock_debug)
		__memblock_dump_all();
}
phys_addr_t memblock_alloc(phys_addr_t size, phys_addr_t align);
int memblock_free(phys_addr_t base, phys_addr_t size);

/**
 * for_each_mem_range_rev - reverse iterate through memblock areas from
 * type_a and not included in type_b. Or just type_a if type_b is NULL.
 * @i: u64 used as loop variable
 * @type_a: ptr to memblock_type to iterate
 * @type_b: ptr to memblock_type which excludes from the iteration
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 */
#define for_each_mem_range_rev(i, type_a, type_b, nid, flags,		\
			       p_start, p_end, p_nid)			\
	for (i = (u64)ULLONG_MAX,					\
		     __next_mem_range_rev(&i, nid, flags, type_a, type_b,\
					 p_start, p_end, p_nid);	\
	     i != (u64)ULLONG_MAX;					\
	     __next_mem_range_rev(&i, nid, flags, type_a, type_b,	\
				  p_start, p_end, p_nid))
/**
 * for_each_free_mem_range_reverse - rev-iterate through free memblock areas
 * @i: u64 used as loop variable
 * @nid: node selector, %NUMA_NO_NODE for all nodes
 * @flags: pick from blocks based on memory attributes
 * @p_start: ptr to phys_addr_t for start address of the range, can be %NULL
 * @p_end: ptr to phys_addr_t for end address of the range, can be %NULL
 * @p_nid: ptr to int for nid of the range, can be %NULL
 *
 * Walks over free (memory && !reserved) areas of memblock in reverse
 * order.  Available as soon as memblock is initialized.
 */
#define for_each_free_mem_range_reverse(i, nid, flags, p_start, p_end,	\
					p_nid)				\
	for_each_mem_range_rev(i, &memblock.memory, &memblock.reserved,	\
			       nid, flags, p_start, p_end, p_nid)

#endif /* _LINUX_MEMBLOCK_H */
