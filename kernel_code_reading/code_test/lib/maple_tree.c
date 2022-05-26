#include "maple_tree.h"

int mtree_insert(struct maple_tree *mt, unsigned long index, void *entry,
		 gfp_t gfp)
{
	return 0;
	// return mtree_insert_range(mt, index, index, entry, gfp);
}
