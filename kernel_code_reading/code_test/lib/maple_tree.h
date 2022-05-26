#ifndef _MAPLE_TREE_H
#define _MAPLE_TREE_H

#include <stdlib.h>
#include <stdbool.h>

/* General kernel definition */
typedef unsigned int gfp_t;
#define	EINVAL		22	/* Invalid argument */

/* Real Start */

#define MAPLE_NODE_SLOTS	31	/* 256 bytes including ->parent */
#define MAPLE_RANGE64_SLOTS	16	/* 256 bytes */
#define MAPLE_ARANGE64_SLOTS	10	/* 240 bytes */
#define MAPLE_ARANGE64_META_MAX	15	/* Out of range for metadata */
#define MAPLE_ALLOC_SLOTS	(MAPLE_NODE_SLOTS - 1)

typedef struct maple_enode *maple_enode; /* encoded node */
typedef struct maple_pnode *maple_pnode; /* parent node */

struct maple_alloc {
	unsigned long total;
	unsigned char node_count;
	unsigned int request_count;
	struct maple_alloc *slot[MAPLE_ALLOC_SLOTS];
};

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

/* Advanced API */

struct ma_state {
	struct maple_tree *tree;	/* The tree we're operating in */
	unsigned long index;		/* The index we're operating on - range start */
	unsigned long last;		/* The last index we're operating on - range end */
	struct maple_enode *node;	/* The node containing this entry */
	unsigned long min;		/* The minimum index of this node - implied pivot min */
	unsigned long max;		/* The maximum index of this node - implied pivot max */
	struct maple_alloc *alloc;	/* Allocated nodes for this operation */
	unsigned char depth;		/* depth of tree descent during write */
	unsigned char offset;
	unsigned char mas_flags;
};

/*
 * Special values for ma_state.node.
 * MAS_START means we have not searched the tree.
 * MAS_ROOT means we have searched the tree and the entry we found lives in
 * the root of the tree (ie it has index 0, length 1 and is the only entry in
 * the tree).
 * MAS_NONE means we have searched the tree and there is no node in the
 * tree for this entry.  For example, we searched for index 1 in an empty
 * tree.  Or we have a tree which points to a full leaf node and we
 * searched for an entry which is larger than can be contained in that
 * leaf node.
 * MA_ERROR represents an errno.  After dropping the lock and attempting
 * to resolve the error, the walk would have to be restarted from the
 * top of the tree as the tree may have been modified.
 */
#define MAS_START	((struct maple_enode *)1UL)
#define MAS_ROOT	((struct maple_enode *)5UL)
#define MAS_NONE	((struct maple_enode *)9UL)
#define MAS_PAUSE	((struct maple_enode *)17UL)
#define MA_ERROR(err) \
		((struct maple_enode *)(((unsigned long)err << 2) | 2UL))

#define MA_STATE(name, mt, first, end)					\
	struct ma_state name = {					\
		.tree = mt,						\
		.index = first,						\
		.last = end,						\
		.node = MAS_START,					\
		.min = 0,						\
		.max = ULONG_MAX,					\
	}

#endif //_MAPLE_TREE_H
