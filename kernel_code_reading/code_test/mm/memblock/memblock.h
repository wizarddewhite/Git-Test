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

#define ULLONG_MAX	(~0ULL)
#define MAX_NUMNODES    (1 << 3)

typedef unsigned long phys_addr_t ;

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

int memblock_add_range(struct memblock_type *type,
				phys_addr_t base, phys_addr_t size,
				int nid, unsigned long flags);
void memblock_dump(struct memblock_type *type, char *name);
int memblock_search(struct memblock_type *type, phys_addr_t addr);
int memblock_remove_range(struct memblock_type *type,
					  phys_addr_t base, phys_addr_t size);

extern struct memblock memblock;
extern int memblock_debug;
extern void __memblock_dump_all(void);
static inline void memblock_dump_all(void)
{
	if (memblock_debug)
		__memblock_dump_all();
}

#endif /* _LINUX_MEMBLOCK_H */
