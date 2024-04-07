#ifndef _IOVA_
#define _IOVA_

#include "rb_tree.h"

#define PAGE_SHIFT	12
#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))

#define VTD_PAGE_SHIFT		(12)
#define VTD_PAGE_SIZE		(1UL << VTD_PAGE_SHIFT)

/* IO virtual address start page frame number */
#define IOVA_START_PFN		(1)
#define IOVA_PFN(addr)		((addr) >> PAGE_SHIFT)
#define DMA_32BIT_PFN		IOVA_PFN(DMA_BIT_MASK(32))
#define DMA_64BIT_PFN		IOVA_PFN(DMA_BIT_MASK(64))

#define IOAPIC_RANGE_START	(0xfee00000)
#define IOAPIC_RANGE_END	(0xfeefffff)

/* iova structure */
struct iova {
	struct rb_node	node;
	unsigned long	pfn_hi; /* Highest allocated pfn */
	unsigned long	pfn_lo; /* Lowest allocated pfn */
};

/* holds all the iova translations for a domain */
struct iova_domain {
	//spinlock_t	iova_rbtree_lock; /* Lock to protect update of rbtree */
	struct rb_root	rbroot;		/* iova domain rbtree root */
	struct rb_node	*cached32_node; /* Save last alloced node */
	unsigned long	granule;	/* pfn granularity for this domain */
	unsigned long	start_pfn;	/* Lower limit for this domain */
	unsigned long	dma_32bit_pfn;
	//struct iova_rcache rcaches[IOVA_RANGE_CACHE_MAX_SIZE];	/* IOVA range caches */
};

#endif //_IOVA_
