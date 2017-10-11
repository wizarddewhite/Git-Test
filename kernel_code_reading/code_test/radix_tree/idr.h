#ifndef __IDR_H__
#define __IDR_H__

#include "radix_tree.h"

struct idr {
	struct radix_tree_root	idr_rt;
	unsigned int		idr_next;
};

#define IDR_FREE	0

/* Set the IDR flag and the IDR_FREE tag */
#define IDR_RT_MARKER		((gfp_t)(3 << __GFP_BITS_SHIFT))

static inline void idr_init(struct idr *idr)
{
	INIT_RADIX_TREE(&idr->idr_rt, IDR_RT_MARKER);
	idr->idr_next = 0;
}

int idr_alloc(struct idr *idr, void *ptr, int start, int end, gfp_t gfp);
#endif //__IDR_H__
