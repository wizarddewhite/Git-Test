#ifndef _MAPLE_TREE_H
#define _MAPLE_TREE_H

#include <stdlib.h>
#include <stdbool.h>

typedef unsigned int gfp_t;

/*
 * If the tree contains a single entry at index 0, it is usually stored in
 * tree->ma_root.  To optimise for the page cache, an entry which ends in '00',
 * '01' or '11' is stored in the root, but an entry which ends in '10' will be
 * stored in a node.  Bits 3-6 are used to store enum maple_type.
 *
 * The flags are used both to store some immutable information about this tree
 * (set at tree creation time) and dynamic information set under the spinlock.
 *
 * Another use of flags are to indicate global states of the tree.  This is the
 * case with the MAPLE_USE_RCU flag, which indicates the tree is currently in
 * RCU mode.  This mode was added to allow the tree to reuse nodes instead of
 * re-allocating and RCU freeing nodes when there is a single user.
 */
struct maple_tree {
	union {
		// spinlock_t	ma_lock;
		// lockdep_map_p	ma_external_lock;
	};
	void /*__rcu*/ *ma_root;
	unsigned int	ma_flags;
};

/**
 * MTREE_INIT() - Initialize a maple tree
 * @name: The maple tree name
 * @flags: The maple tree flags
 *
 */
#define MTREE_INIT(name, flags) {					\
	/*.ma_lock = __SPIN_LOCK_UNLOCKED(name.ma_lock),*/		\
	.ma_flags = flags,						\
	.ma_root = NULL,						\
}

#define DEFINE_MTREE(name)						\
	struct maple_tree name = MTREE_INIT(name, 0)

#define mtree_lock(mt)		// spin_lock((&(mt)->ma_lock))
#define mtree_unlock(mt)	// spin_unlock((&(mt)->ma_lock))

/**
 * mtree_empty() - Determine if a tree has any present entries.
 * @mt: Maple Tree.
 *
 * Context: Any context.
 * Return: %true if the tree contains only NULL pointers.
 */
static inline bool mtree_empty(const struct maple_tree *mt)
{
	return mt->ma_root == NULL;
}


int mtree_insert(struct maple_tree *mt, unsigned long index,
		void *entry, gfp_t gfp);
int mtree_insert_range(struct maple_tree *mt, unsigned long first,
		unsigned long last, void *entry, gfp_t gfp);
#endif //_MAPLE_TREE_H
