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
#define ULLONG_MAX	(~0ULL)

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
#endif /* _LINUX_MEMBLOCK_H */
