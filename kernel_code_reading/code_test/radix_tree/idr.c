#include "radix_tree.h"
#include "idr.h"

int idr_alloc(struct idr *idr, void *ptr, int start, int end, gfp_t gfp)
{
	void **slot;
	struct radix_tree_iter iter;

	if ((start < 0))
		return -1;
	if ((radix_tree_is_internal_node(ptr)))
		return -1;

	radix_tree_iter_init(&iter, start);
	slot = idr_get_free(&idr->idr_rt, &iter, gfp, end);
	if (!(slot))
		return -1;//PTR_ERR(slot);

	radix_tree_iter_replace(&idr->idr_rt, &iter, slot, ptr);
	radix_tree_iter_tag_clear(&idr->idr_rt, &iter, IDR_FREE);
	return iter.index;
}
