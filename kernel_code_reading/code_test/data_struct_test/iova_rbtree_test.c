#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iova.h"

static struct iova_domain reserved_iova_list;

void iova_print(struct rb_node *node, char *prefix, int level)
{
	struct iova *this;

	this = container_of(node, struct iova, node);
	printf("%02d %s -[%016lx, %016lx](%c)\n",
			level, prefix, this->pfn_lo, this->pfn_hi,
			rb_is_red(node)?'r':'b');
}

void
init_iova_domain(struct iova_domain *iovad, unsigned long granule,
	unsigned long start_pfn, unsigned long pfn_32bit)
{
	/*
	 * IOVA granularity will normally be equal to the smallest
	 * supported IOMMU page size; both *must* be capable of
	 * representing individual CPU pages exactly.
	 */
	//BUG_ON((granule > PAGE_SIZE) || !is_power_of_2(granule));

	//spin_lock_init(&iovad->iova_rbtree_lock);
	iovad->rbroot = RB_ROOT;
	iovad->cached32_node = NULL;
	iovad->granule = granule;
	iovad->start_pfn = start_pfn;
	iovad->dma_32bit_pfn = pfn_32bit;
	//init_iova_rcaches(iovad);
}

static int
__is_range_overlap(struct rb_node *node,
	unsigned long pfn_lo, unsigned long pfn_hi)
{
	struct iova *iova = container_of(node, struct iova, node);

	if ((pfn_lo <= iova->pfn_hi) && (pfn_hi >= iova->pfn_lo))
		return 1;
	return 0;
}

static void
__adjust_overlap_range(struct iova *iova,
	unsigned long *pfn_lo, unsigned long *pfn_hi)
{
	if (*pfn_lo < iova->pfn_lo)
		iova->pfn_lo = *pfn_lo;
	if (*pfn_hi > iova->pfn_hi)
		*pfn_lo = iova->pfn_hi + 1;
}

static inline struct iova *
alloc_and_init_iova(unsigned long pfn_lo, unsigned long pfn_hi)
{
	struct iova *iova;

	iova = malloc(sizeof(struct iova));
	if (iova) {
		iova->pfn_lo = pfn_lo;
		iova->pfn_hi = pfn_hi;
	}

	return iova;
}

static void
iova_insert_rbtree(struct rb_root *root, struct iova *iova)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	/* Figure out where to put new node */
	while (*new) {
		struct iova *this = container_of(*new, struct iova, node);

		parent = *new;

		if (iova->pfn_lo < this->pfn_lo)
			new = &((*new)->rb_left);
		else if (iova->pfn_lo > this->pfn_lo)
			new = &((*new)->rb_right);
		else
			exit(-1); /* this should not happen */
	}
	/* Add new node and rebalance tree. */
	rb_link_node(&iova->node, parent, new);
	rb_insert_color(&iova->node, root);
}

static struct iova *
__insert_new_range(struct iova_domain *iovad,
	unsigned long pfn_lo, unsigned long pfn_hi)
{
	struct iova *iova;

	iova = alloc_and_init_iova(pfn_lo, pfn_hi);
	if (iova)
		iova_insert_rbtree(&iovad->rbroot, iova);

	return iova;
}

struct iova *
reserve_iova(struct iova_domain *iovad,
	unsigned long pfn_lo, unsigned long pfn_hi)
{
	struct rb_node *node;
	unsigned long flags;
	struct iova *iova;
	unsigned int overlap = 0;

	//spin_lock_irqsave(&iovad->iova_rbtree_lock, flags);
	for (node = rb_first(&iovad->rbroot); node; node = rb_next(node)) {
		if (__is_range_overlap(node, pfn_lo, pfn_hi)) {
			iova = container_of(node, struct iova, node);
			__adjust_overlap_range(iova, &pfn_lo, &pfn_hi);
			if ((pfn_lo >= iova->pfn_lo) &&
				(pfn_hi <= iova->pfn_hi))
				goto finish;
			overlap = 1;

		} else if (overlap)
				break;
	}

	/* We are here either because this is the first reserver node
	 * or need to insert remaining non overlap addr range
	 */
	iova = __insert_new_range(iovad, pfn_lo, pfn_hi);
finish:

	//spin_unlock_irqrestore(&iovad->iova_rbtree_lock, flags);
	return iova;
}
int main()
{
	struct iova *iova;
	init_iova_domain(&reserved_iova_list, VTD_PAGE_SIZE, IOVA_START_PFN,
			DMA_32BIT_PFN);

	iova = reserve_iova(&reserved_iova_list, IOVA_PFN(IOAPIC_RANGE_START),
		IOVA_PFN(IOAPIC_RANGE_END));

	dump_rb_tree(reserved_iova_list.rbroot.rb_node, 0, root_node, iova_print);

	iova = reserve_iova(&reserved_iova_list, IOVA_PFN(IOAPIC_RANGE_START - 0x100000),
		IOVA_PFN(IOAPIC_RANGE_END));
	dump_rb_tree(reserved_iova_list.rbroot.rb_node, 0, root_node, iova_print);

	iova = reserve_iova(&reserved_iova_list, IOVA_PFN(IOAPIC_RANGE_START - 0x100000),
		IOVA_PFN(IOAPIC_RANGE_END + 0x100000));
	dump_rb_tree(reserved_iova_list.rbroot.rb_node, 0, root_node, iova_print);

	return 0;
}
