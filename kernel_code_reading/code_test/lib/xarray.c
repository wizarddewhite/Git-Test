#include "xarray.h"
#include "bug.h"
#include "bitmap.h"

/*
 * Coding conventions in this file:
 *
 * @xa is used to refer to the entire xarray.
 * @xas is the 'xarray operation state'.  It may be either a pointer to
 * an xa_state, or an xa_state stored on the stack.  This is an unfortunate
 * ambiguity.
 * @index is the index of the entry being operated on
 * @mark is an xa_mark_t; a small number indicating one of the mark bits.
 * @node refers to an xa_node; usually the primary one being operated on by
 * this function.
 * @offset is the index into the slots array inside an xa_node.
 * @parent refers to the @xa_node closer to the head than @node.
 * @entry refers to something stored in a slot in the xarray
 */

static inline unsigned int xa_lock_type(const struct xarray *xa)
{
	return (__force unsigned int)xa->xa_flags & 3;
}

static inline void xas_lock_type(struct xa_state *xas, unsigned int lock_type)
{
	if (lock_type == XA_LOCK_IRQ)
		xas_lock_irq(xas);
	else if (lock_type == XA_LOCK_BH)
		xas_lock_bh(xas);
	else
		xas_lock(xas);
}

static inline void xas_unlock_type(struct xa_state *xas, unsigned int lock_type)
{
	if (lock_type == XA_LOCK_IRQ)
		xas_unlock_irq(xas);
	else if (lock_type == XA_LOCK_BH)
		xas_unlock_bh(xas);
	else
		xas_unlock(xas);
}

static inline bool xa_track_free(const struct xarray *xa)
{
	return xa->xa_flags & XA_FLAGS_TRACK_FREE;
}

static inline bool xa_zero_busy(const struct xarray *xa)
{
	return xa->xa_flags & XA_FLAGS_ZERO_BUSY;
}

static inline void xa_mark_set(struct xarray *xa, xa_mark_t mark)
{
	if (!(xa->xa_flags & XA_FLAGS_MARK(mark)))
		xa->xa_flags |= XA_FLAGS_MARK(mark);
}

static inline void xa_mark_clear(struct xarray *xa, xa_mark_t mark)
{
	if (xa->xa_flags & XA_FLAGS_MARK(mark))
		xa->xa_flags &= ~(XA_FLAGS_MARK(mark));
}

static inline unsigned long *node_marks(struct xa_node *node, xa_mark_t mark)
{
	return node->marks[(__force unsigned)mark];
}

static inline bool node_get_mark(struct xa_node *node,
		unsigned int offset, xa_mark_t mark)
{
	return test_bit(offset, node_marks(node, mark));
}

/* returns true if the bit was set */
static inline bool node_set_mark(struct xa_node *node, unsigned int offset,
				xa_mark_t mark)
{
	return __test_and_set_bit(offset, node_marks(node, mark));
}

/* returns true if the bit was set */
static inline bool node_clear_mark(struct xa_node *node, unsigned int offset,
				xa_mark_t mark)
{
	return __test_and_clear_bit(offset, node_marks(node, mark));
}

static inline bool node_any_mark(struct xa_node *node, xa_mark_t mark)
{
	return !bitmap_empty(node_marks(node, mark), XA_CHUNK_SIZE);
}

static inline void node_mark_all(struct xa_node *node, xa_mark_t mark)
{
	bitmap_fill(node_marks(node, mark), XA_CHUNK_SIZE);
}

#define mark_inc(mark) do { \
	mark = (__force xa_mark_t)((__force unsigned)(mark) + 1); \
} while (0)

/*
 * xas_squash_marks() - Merge all marks to the first entry
 * @xas: Array operation state.
 *
 * Set a mark on the first entry if any entry has it set.  Clear marks on
 * all sibling entries.
 */
static void xas_squash_marks(const struct xa_state *xas)
{
	unsigned int mark = 0;
	unsigned int limit = xas->xa_offset + xas->xa_sibs + 1;

	if (!xas->xa_sibs)
		return;

	do {
		unsigned long *marks = xas->xa_node->marks[mark];
		if (find_next_bit(marks, limit, xas->xa_offset + 1) == limit)
			continue;
		__set_bit(xas->xa_offset, marks);
		bitmap_clear(marks, xas->xa_offset + 1, xas->xa_sibs);
	} while (mark++ != (__force unsigned)XA_MARK_MAX);
}

/* extracts the offset within this node from the index */
static unsigned int get_offset(unsigned long index, struct xa_node *node)
{
	return (index >> node->shift) & XA_CHUNK_MASK;
}

static void xas_set_offset(struct xa_state *xas)
{
	xas->xa_offset = get_offset(xas->xa_index, xas->xa_node);
}

/* move the index either forwards (find) or backwards (sibling slot) */
static void xas_move_index(struct xa_state *xas, unsigned long offset)
{
	unsigned int shift = xas->xa_node->shift;
	xas->xa_index &= ~XA_CHUNK_MASK << shift;
	xas->xa_index += offset << shift;
}

static void xas_next_offset(struct xa_state *xas)
{
	xas->xa_offset++;
	xas_move_index(xas, xas->xa_offset);
}

static void *set_bounds(struct xa_state *xas)
{
	xas->xa_node = XAS_BOUNDS;
	return NULL;
}

/*
 * Starts a walk.  If the @xas is already valid, we assume that it's on
 * the right path and just return where we've got to.  If we're in an
 * error state, return NULL.  If the index is outside the current scope
 * of the xarray, return NULL without changing @xas->xa_node.  Otherwise
 * set @xas->xa_node to NULL and return the current head of the array.
 */
static void *xas_start(struct xa_state *xas)
{
	void *entry;

	if (xas_valid(xas))
		return xas_reload(xas);
	if (xas_error(xas))
		return NULL;

	entry = xa_head(xas->xa);
	if (!xa_is_node(entry)) {
		if (xas->xa_index)
			return set_bounds(xas);
	} else {
		if ((xas->xa_index >> xa_to_node(entry)->shift) > XA_CHUNK_MASK)
			return set_bounds(xas);
	}

	xas->xa_node = NULL;
	return entry;
}

static void *xas_descend(struct xa_state *xas, struct xa_node *node)
{
	unsigned int offset = get_offset(xas->xa_index, node);
	void *entry = xa_entry(xas->xa, node, offset);

	xas->xa_node = node;
	if (xa_is_sibling(entry)) {
		offset = xa_to_sibling(entry);
		entry = xa_entry(xas->xa, node, offset);
		if (node->shift && xa_is_node(entry))
			entry = XA_RETRY_ENTRY;
	}

	xas->xa_offset = offset;
	return entry;
}
/**
 * xas_load() - Load an entry from the XArray (advanced).
 * @xas: XArray operation state.
 *
 * Usually walks the @xas to the appropriate state to load the entry
 * stored at xa_index.  However, it will do nothing and return %NULL if
 * @xas is in an error state.  xas_load() will never expand the tree.
 *
 * If the xa_state is set up to operate on a multi-index entry, xas_load()
 * may return %NULL or an internal entry, even if there are entries
 * present within the range specified by @xas.
 *
 * Context: Any context.  The caller should hold the xa_lock or the RCU lock.
 * Return: Usually an entry in the XArray, but see description for exceptions.
 */
void *xas_load(struct xa_state *xas)
{
	void *entry = xas_start(xas);

	while (xa_is_node(entry)) {
		struct xa_node *node = xa_to_node(entry);

		if (xas->xa_shift > node->shift)
			break;
		entry = xas_descend(xas, node);
		if (node->shift == 0)
			break;
	}
	return entry;
}

#define XA_RCU_FREE	((struct xarray *)1)

#define radix_tree_node		xa_node
void radix_tree_node_rcu_free(struct rcu_head *head)
{
	struct radix_tree_node *node =
			container_of(head, struct radix_tree_node, rcu_head);

	/*
	 * Must only free zeroed nodes into the slab.  We can be left with
	 * non-NULL entries by radix_tree_free_nodes, so clear the entries
	 * and tags here.
	 */
	memset(node->slots, 0, sizeof(node->slots));
	memset(node->tags, 0, sizeof(node->tags));
	INIT_LIST_HEAD(&node->private_list);

	// kmem_cache_free(radix_tree_node_cachep, node);
	free(node);
}

static void xa_node_free(struct xa_node *node)
{
	XA_NODE_BUG_ON(node, !list_empty(&node->private_list));
	node->array = XA_RCU_FREE;
	// call_rcu(&node->rcu_head, radix_tree_node_rcu_free);
	radix_tree_node_rcu_free(&node->rcu_head);
}

/*
 * xas_destroy() - Free any resources allocated during the XArray operation.
 * @xas: XArray operation state.
 *
 * This function is now internal-only.
 */
static void xas_destroy(struct xa_state *xas)
{
	struct xa_node *next, *node = xas->xa_alloc;

	while (node) {
		XA_NODE_BUG_ON(node, !list_empty(&node->private_list));
		next = /* rcu_dereference_raw */(node->parent);
		radix_tree_node_rcu_free(&node->rcu_head);
		xas->xa_alloc = node = next;
	}
}

/**
 * xas_nomem() - Allocate memory if needed.
 * @xas: XArray operation state.
 * @gfp: Memory allocation flags.
 *
 * If we need to add new nodes to the XArray, we try to allocate memory
 * with GFP_NOWAIT while holding the lock, which will usually succeed.
 * If it fails, @xas is flagged as needing memory to continue.  The caller
 * should drop the lock and call xas_nomem().  If xas_nomem() succeeds,
 * the caller should retry the operation.
 *
 * Forward progress is guaranteed as one node is allocated here and
 * stored in the xa_state where it will be found by xas_alloc().  More
 * nodes will likely be found in the slab allocator, but we do not tie
 * them up here.
 *
 * Return: true if memory was needed, and was successfully allocated.
 */
bool xas_nomem(struct xa_state *xas, gfp_t gfp)
{
	if (xas->xa_node != XA_ERROR(-ENOMEM)) {
		xas_destroy(xas);
		return false;
	}
	// if (xas->xa->xa_flags & XA_FLAGS_ACCOUNT)
	// 	gfp |= __GFP_ACCOUNT;
	// xas->xa_alloc = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
	xas->xa_alloc = malloc(sizeof(struct xa_node));
	if (!xas->xa_alloc)
		return false;
	memset(xas->xa_alloc, 0, sizeof(struct xa_node));
	INIT_LIST_HEAD(&xas->xa_alloc->private_list);
	xas->xa_alloc->parent = NULL;
	XA_NODE_BUG_ON(xas->xa_alloc, !list_empty(&xas->xa_alloc->private_list));
	xas->xa_node = XAS_RESTART;
	return true;
}

/*
 * __xas_nomem() - Drop locks and allocate memory if needed.
 * @xas: XArray operation state.
 * @gfp: Memory allocation flags.
 *
 * Internal variant of xas_nomem().
 *
 * Return: true if memory was needed, and was successfully allocated.
 */
static bool __xas_nomem(struct xa_state *xas, gfp_t gfp)
	// __must_hold(xas->xa->xa_lock)
{
	unsigned int lock_type = xa_lock_type(xas->xa);

	if (xas->xa_node != XA_ERROR(-ENOMEM)) {
		xas_destroy(xas);
		return false;
	}
	// if (xas->xa->xa_flags & XA_FLAGS_ACCOUNT)
	// 	gfp |= __GFP_ACCOUNT;
	// if (gfpflags_allow_blocking(gfp)) {
	// 	xas_unlock_type(xas, lock_type);
	// 	xas->xa_alloc = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
	// 	xas_lock_type(xas, lock_type);
	// } else {
	//	xas->xa_alloc = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
	// }
	xas->xa_alloc = malloc(sizeof(struct xa_node));
	if (!xas->xa_alloc)
		return false;
	memset(xas->xa_alloc, 0, sizeof(struct xa_node));
	INIT_LIST_HEAD(&xas->xa_alloc->private_list);
	xas->xa_alloc->parent = NULL;
	XA_NODE_BUG_ON(xas->xa_alloc, !list_empty(&xas->xa_alloc->private_list));
	xas->xa_node = XAS_RESTART;
	return true;
}

static void xas_update(struct xa_state *xas, struct xa_node *node)
{
	if (xas->xa_update)
		xas->xa_update(node);
	else
		XA_NODE_BUG_ON(node, !list_empty(&node->private_list));
}

static void *xas_alloc(struct xa_state *xas, unsigned int shift)
{
	struct xa_node *parent = xas->xa_node;
	struct xa_node *node = xas->xa_alloc;

	if (xas_invalid(xas))
		return NULL;

	if (node) {
		xas->xa_alloc = NULL;
	} else {
		// gfp_t gfp = GFP_NOWAIT | __GFP_NOWARN;

		// if (xas->xa->xa_flags & XA_FLAGS_ACCOUNT)
		// 	gfp |= __GFP_ACCOUNT;

		// node = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
		node = malloc(sizeof(struct xa_node));
		if (!node) {
			xas_set_err(xas, -ENOMEM);
			return NULL;
		}
		memset(node, 0, sizeof(struct xa_node));
		INIT_LIST_HEAD(&node->private_list);
	}

	if (parent) {
		node->offset = xas->xa_offset;
		parent->count++;
		XA_NODE_BUG_ON(node, parent->count > XA_CHUNK_SIZE);
		xas_update(xas, parent);
	}
	XA_NODE_BUG_ON(node, shift > BITS_PER_LONG);
	XA_NODE_BUG_ON(node, !list_empty(&node->private_list));
	node->shift = shift;
	node->count = 0;
	node->nr_values = 0;
	// RCU_INIT_POINTER(node->parent, xas->xa_node);
	node->parent = xas->xa_node;
	node->array = xas->xa;

	return node;
}

#ifdef CONFIG_XARRAY_MULTI
/* Returns the number of indices covered by a given xa_state */
unsigned long xas_size(const struct xa_state *xas)
{
	return (xas->xa_sibs + 1UL) << xas->xa_shift;
}
#endif

/*
 * Use this to calculate the maximum index that will need to be created
 * in order to add the entry described by @xas.  Because we cannot store a
 * multi-index entry at index 0, the calculation is a little more complex
 * than you might expect.
 */
unsigned long xas_max(struct xa_state *xas)
{
	unsigned long max = xas->xa_index;

#ifdef CONFIG_XARRAY_MULTI
	if (xas->xa_shift || xas->xa_sibs) {
		unsigned long mask = xas_size(xas) - 1;
		max |= mask;
		if (mask == max) {
			max++;
		}
	}
#endif

	return max;
}

/* The maximum index that can be contained in the array without expanding it */
static unsigned long max_index(void *entry)
{
	if (!xa_is_node(entry))
		return 0;
	return (XA_CHUNK_SIZE << xa_to_node(entry)->shift) - 1;
}

static void xas_shrink(struct xa_state *xas)
{
	struct xarray *xa = xas->xa;
	struct xa_node *node = xas->xa_node;

	for (;;) {
		void *entry;

		XA_NODE_BUG_ON(node, node->count > XA_CHUNK_SIZE);
		if (node->count != 1)
			break;
		entry = xa_entry_locked(xa, node, 0);
		if (!entry)
			break;
		if (!xa_is_node(entry) && node->shift)
			break;
		if (xa_is_zero(entry) && xa_zero_busy(xa))
			entry = NULL;
		xas->xa_node = XAS_BOUNDS;

		// RCU_INIT_POINTER(xa->xa_head, entry);
		xa->xa_head = entry;
		if (xa_track_free(xa) && !node_get_mark(node, 0, XA_FREE_MARK))
			xa_mark_clear(xa, XA_FREE_MARK);

		node->count = 0;
		node->nr_values = 0;
		if (!xa_is_node(entry))
			// RCU_INIT_POINTER(node->slots[0], XA_RETRY_ENTRY);
			node->slots[0] = XA_RETRY_ENTRY;
		xas_update(xas, node);
		xa_node_free(node);
		if (!xa_is_node(entry))
			break;
		node = xa_to_node(entry);
		node->parent = NULL;
	}
}

/*
 * xas_delete_node() - Attempt to delete an xa_node
 * @xas: Array operation state.
 *
 * Attempts to delete the @xas->xa_node.  This will fail if xa->node has
 * a non-zero reference count.
 */
static void xas_delete_node(struct xa_state *xas)
{
	struct xa_node *node = xas->xa_node;

	for (;;) {
		struct xa_node *parent;

		XA_NODE_BUG_ON(node, node->count > XA_CHUNK_SIZE);
		if (node->count)
			break;

		parent = xa_parent_locked(xas->xa, node);
		xas->xa_node = parent;
		xas->xa_offset = node->offset;
		xa_node_free(node);

		if (!parent) {
			xas->xa->xa_head = NULL;
			xas->xa_node = XAS_BOUNDS;
			return;
		}

		parent->slots[xas->xa_offset] = NULL;
		parent->count--;
		XA_NODE_BUG_ON(parent, parent->count > XA_CHUNK_SIZE);
		node = parent;
		xas_update(xas, node);
	}

	if (!node->parent)
		xas_shrink(xas);
}

/**
 * xas_free_nodes() - Free this node and all nodes that it references
 * @xas: Array operation state.
 * @top: Node to free
 *
 * This node has been removed from the tree.  We must now free it and all
 * of its subnodes.  There may be RCU walkers with references into the tree,
 * so we must replace all entries with retry markers.
 */
static void xas_free_nodes(struct xa_state *xas, struct xa_node *top)
{
	unsigned int offset = 0;
	struct xa_node *node = top;

	for (;;) {
		void *entry = xa_entry_locked(xas->xa, node, offset);

		if (node->shift && xa_is_node(entry)) {
			node = xa_to_node(entry);
			offset = 0;
			continue;
		}
		if (entry)
			// RCU_INIT_POINTER(node->slots[offset], XA_RETRY_ENTRY);
			node->slots[offset] = XA_RETRY_ENTRY;
		offset++;
		while (offset == XA_CHUNK_SIZE) {
			struct xa_node *parent;

			parent = xa_parent_locked(xas->xa, node);
			offset = node->offset + 1;
			node->count = 0;
			node->nr_values = 0;
			xas_update(xas, node);
			xa_node_free(node);
			if (node == top)
				return;
			node = parent;
		}
	}
}

/*
 * xas_expand adds nodes to the head of the tree until it has reached
 * sufficient height to be able to contain @xas->xa_index
 */
static int xas_expand(struct xa_state *xas, void *head)
{
	struct xarray *xa = xas->xa;
	struct xa_node *node = NULL;
	unsigned int shift = 0;
	unsigned long max = xas_max(xas);

	if (!head) {
		if (max == 0)
			return 0;
		while ((max >> shift) >= XA_CHUNK_SIZE)
			shift += XA_CHUNK_SHIFT;
		return shift + XA_CHUNK_SHIFT;
	} else if (xa_is_node(head)) {
		node = xa_to_node(head);
		shift = node->shift + XA_CHUNK_SHIFT;
	}
	xas->xa_node = NULL;

	while (max > max_index(head)) {
		xa_mark_t mark = 0;

		XA_NODE_BUG_ON(node, shift > BITS_PER_LONG);
		node = xas_alloc(xas, shift);
		if (!node)
			return -ENOMEM;

		node->count = 1;
		if (xa_is_value(head))
			node->nr_values = 1;
		// RCU_INIT_POINTER(node->slots[0], head);
		node->slots[0] = head;

		/* Propagate the aggregated mark info to the new child */
		for (;;) {
			if (xa_track_free(xa) && mark == XA_FREE_MARK) {
				node_mark_all(node, XA_FREE_MARK);
				if (!xa_marked(xa, XA_FREE_MARK)) {
					node_clear_mark(node, 0, XA_FREE_MARK);
					xa_mark_set(xa, XA_FREE_MARK);
				}
			} else if (xa_marked(xa, mark)) {
				node_set_mark(node, 0, mark);
			}
			if (mark == XA_MARK_MAX)
				break;
			mark_inc(mark);
		}

		/*
		 * Now that the new node is fully initialised, we can add
		 * it to the tree
		 */
		if (xa_is_node(head)) {
			xa_to_node(head)->offset = 0;
			// rcu_assign_pointer(xa_to_node(head)->parent, node);
			xa_to_node(head)->parent = node;
		}
		head = xa_mk_node(node);
		xa->xa_head = head;
		xas_update(xas, node);

		shift += XA_CHUNK_SHIFT;
	}

	xas->xa_node = node;
	return shift;
}

/*
 * xas_create() - Create a slot to store an entry in.
 * @xas: XArray operation state.
 * @allow_root: %true if we can store the entry in the root directly
 *
 * Most users will not need to call this function directly, as it is called
 * by xas_store().  It is useful for doing conditional store operations
 * (see the xa_cmpxchg() implementation for an example).
 *
 * Return: If the slot already existed, returns the contents of this slot.
 * If the slot was newly created, returns %NULL.  If it failed to create the
 * slot, returns %NULL and indicates the error in @xas.
 */
static void *xas_create(struct xa_state *xas, bool allow_root)
{
	struct xarray *xa = xas->xa;
	void *entry;
	void **slot;
	struct xa_node *node = xas->xa_node;
	int shift;
	unsigned int order = xas->xa_shift;

	if (xas_top(node)) {
		entry = xa_head_locked(xa);
		xas->xa_node = NULL;
		if (!entry && xa_zero_busy(xa))
			entry = XA_ZERO_ENTRY;
		shift = xas_expand(xas, entry);
		if (shift < 0)
			return NULL;
		if (!shift && !allow_root)
			shift = XA_CHUNK_SHIFT;
		entry = xa_head_locked(xa);
		slot = &xa->xa_head;
	} else if (xas_error(xas)) {
		return NULL;
	} else if (node) {
		unsigned int offset = xas->xa_offset;

		shift = node->shift;
		entry = xa_entry_locked(xa, node, offset);
		slot = &node->slots[offset];

		// printf("\txas_create locate node %p entry %p\n", node, entry);
	} else {
		shift = 0;
		entry = xa_head_locked(xa);
		slot = &xa->xa_head;
	}

	while (shift > order) {
		shift -= XA_CHUNK_SHIFT;
		if (!entry) {
			node = xas_alloc(xas, shift);
			if (!node)
				break;
			if (xa_track_free(xa))
				node_mark_all(node, XA_FREE_MARK);
			// rcu_assign_pointer(*slot, xa_mk_node(node));
			*slot = xa_mk_node(node);
		} else if (xa_is_node(entry)) {
			node = xa_to_node(entry);
		} else {
			// printf("\txas_create and isn't a node \n");
			break;
		}
		entry = xas_descend(xas, node);
		slot = &node->slots[xas->xa_offset];
	}

	return entry;
}

/**
 * xas_create_range() - Ensure that stores to this range will succeed
 * @xas: XArray operation state.
 *
 * Creates all of the slots in the range covered by @xas.  Sets @xas to
 * create single-index entries and positions it at the beginning of the
 * range.  This is for the benefit of users which have not yet been
 * converted to use multi-index entries.
 */
void xas_create_range(struct xa_state *xas)
{
	unsigned long index = xas->xa_index;
	unsigned char shift = xas->xa_shift;
	unsigned char sibs = xas->xa_sibs;
	struct xa_node *node;

	xas->xa_index |= ((sibs + 1UL) << shift) - 1;
	if (xas_is_node(xas) && xas->xa_node->shift == xas->xa_shift)
		xas->xa_offset |= sibs;
	xas->xa_shift = 0;
	xas->xa_sibs = 0;

	int idx = 0;

	for (;;) {
		idx++;
		xas_create(xas, true);
		// xa_dump(xas->xa, false);
		if (xas_error(xas))
			goto restore;
		if (xas->xa_index <= (index | XA_CHUNK_MASK))
			goto success;
#ifdef DEBUG
		printf("index %lu created ", xas->xa_index);
#endif
		xas->xa_index -= XA_CHUNK_SIZE;

#ifdef DEBUG
		printf("look for index %lu from parent: %p(%u),\n",
			xas->xa_index, xas->xa_node, xas->xa_offset);
#endif
		node = xas->xa_node;
			if (node->shift != 0) {

				unsigned next_index = xas->xa_index >> node->shift;
				next_index &= ~XA_CHUNK_MASK;
				next_index += xas->xa_offset;
				next_index <<= node->shift;
#ifdef DEBUG
				printf("...bingo! shift %d\n", shift);
				printf("  expe index %u\n", next_index);
#endif

				if (next_index <= (index & ~XA_CHUNK_MASK))
					goto success;

				xas_set(xas, next_index - 1);
				continue;
			}

		for (;;) {

			xas->xa_node = xa_parent_locked(xas->xa, node);
			if (!xas->xa_node)
				break;

			xas->xa_offset = node->offset - 1;
#ifdef DEBUG
			printf("parent of %p is %p\n", node, xas->xa_node);
#endif
			if (node->offset != 0) {
				break;
			}
			node = xas->xa_node;
		}
#ifdef DEBUG
		printf(" and found: %p(%u)\n", xas->xa_node, xas->xa_offset);
#endif
	}

restore:
	xas->xa_shift = shift;
	xas->xa_sibs = sibs;
	xas->xa_index = index;
	return;
success:
#ifdef DEBUG
	printf("total iterations: %d, xas->xa_shift %d\n", idx, xas->xa_shift);
#endif
	xas->xa_index = index;
	if (xas->xa_node)
		xas_set_offset(xas);
}

static void update_node(struct xa_state *xas, struct xa_node *node,
		int count, int values)
{
	if (!node || (!count && !values))
		return;

	node->count += count;
	node->nr_values += values;
	XA_NODE_BUG_ON(node, node->count > XA_CHUNK_SIZE);
	XA_NODE_BUG_ON(node, node->nr_values > XA_CHUNK_SIZE);
	xas_update(xas, node);
	if (count < 0)
		xas_delete_node(xas);
}

/**
 * xas_store() - Store this entry in the XArray.
 * @xas: XArray operation state.
 * @entry: New entry.
 *
 * If @xas is operating on a multi-index entry, the entry returned by this
 * function is essentially meaningless (it may be an internal entry or it
 * may be %NULL, even if there are non-NULL entries at some of the indices
 * covered by the range).  This is not a problem for any current users,
 * and can be changed if needed.
 *
 * Return: The old entry at this index.
 */
void *xas_store(struct xa_state *xas, void *entry)
{
	struct xa_node *node;
	void **slot = &xas->xa->xa_head;
	unsigned int offset, max;
	int count = 0;
	int values = 0;
	void *first, *next;
	bool value = xa_is_value(entry);

	if (entry) {
		bool allow_root = !xa_is_node(entry) && !xa_is_zero(entry);
		first = xas_create(xas, allow_root);
	} else {
		first = xas_load(xas);
	}

	if (xas_invalid(xas))
		return first;
	node = xas->xa_node;
	if (node && (xas->xa_shift < node->shift))
		xas->xa_sibs = 0;
	if ((first == entry) && !xas->xa_sibs)
		return first;

	next = first;
	offset = xas->xa_offset;
	max = xas->xa_offset + xas->xa_sibs;
	if (node) {
		slot = &node->slots[offset];
		if (xas->xa_sibs)
			xas_squash_marks(xas);
	}
	if (!entry)
		xas_init_marks(xas);

	for (;;) {
		/*
		 * Must clear the marks before setting the entry to NULL,
		 * otherwise xas_for_each_marked may find a NULL entry and
		 * stop early.  rcu_assign_pointer contains a release barrier
		 * so the mark clearing will appear to happen before the
		 * entry is set to NULL.
		 */
		// rcu_assign_pointer(*slot, entry);
		*slot = entry;
		if (xa_is_node(next) && (!node || node->shift))
			xas_free_nodes(xas, xa_to_node(next));
		if (!node)
			break;
		count += !next - !entry;
		values += !xa_is_value(first) - !value;
		if (entry) {
			if (offset == max)
				break;
			if (!xa_is_sibling(entry))
				entry = xa_mk_sibling(xas->xa_offset);
		} else {
			if (offset == XA_CHUNK_MASK)
				break;
		}
		next = xa_entry_locked(xas->xa, node, ++offset);
		if (!xa_is_sibling(next)) {
			if (!entry && (offset > max))
				break;
			first = next;
		}
		slot++;
	}

	update_node(xas, node, count, values);
	return first;
}

/**
 * xas_get_mark() - Returns the state of this mark.
 * @xas: XArray operation state.
 * @mark: Mark number.
 *
 * Return: true if the mark is set, false if the mark is clear or @xas
 * is in an error state.
 */
bool xas_get_mark(const struct xa_state *xas, xa_mark_t mark)
{
	if (xas_invalid(xas))
		return false;
	if (!xas->xa_node)
		return xa_marked(xas->xa, mark);
	return node_get_mark(xas->xa_node, xas->xa_offset, mark);
}

/**
 * xas_set_mark() - Sets the mark on this entry and its parents.
 * @xas: XArray operation state.
 * @mark: Mark number.
 *
 * Sets the specified mark on this entry, and walks up the tree setting it
 * on all the ancestor entries.  Does nothing if @xas has not been walked to
 * an entry, or is in an error state.
 */
void xas_set_mark(const struct xa_state *xas, xa_mark_t mark)
{
	struct xa_node *node = xas->xa_node;
	unsigned int offset = xas->xa_offset;

	if (xas_invalid(xas))
		return;

	while (node) {
		if (node_set_mark(node, offset, mark))
			return;
		offset = node->offset;
		node = xa_parent_locked(xas->xa, node);
	}

	if (!xa_marked(xas->xa, mark))
		xa_mark_set(xas->xa, mark);
}

/**
 * xas_clear_mark() - Clears the mark on this entry and its parents.
 * @xas: XArray operation state.
 * @mark: Mark number.
 *
 * Clears the specified mark on this entry, and walks back to the head
 * attempting to clear it on all the ancestor entries.  Does nothing if
 * @xas has not been walked to an entry, or is in an error state.
 */
void xas_clear_mark(const struct xa_state *xas, xa_mark_t mark)
{
	struct xa_node *node = xas->xa_node;
	unsigned int offset = xas->xa_offset;

	if (xas_invalid(xas))
		return;

	while (node) {
		if (!node_clear_mark(node, offset, mark))
			return;
		if (node_any_mark(node, mark))
			return;

		offset = node->offset;
		node = xa_parent_locked(xas->xa, node);
	}

	if (xa_marked(xas->xa, mark))
		xa_mark_clear(xas->xa, mark);
}

/**
 * xas_init_marks() - Initialise all marks for the entry
 * @xas: Array operations state.
 *
 * Initialise all marks for the entry specified by @xas.  If we're tracking
 * free entries with a mark, we need to set it on all entries.  All other
 * marks are cleared.
 *
 * This implementation is not as efficient as it could be; we may walk
 * up the tree multiple times.
 */
void xas_init_marks(const struct xa_state *xas)
{
	xa_mark_t mark = 0;

	for (;;) {
		if (xa_track_free(xas->xa) && mark == XA_FREE_MARK)
			xas_set_mark(xas, mark);
		else
			xas_clear_mark(xas, mark);
		if (mark == XA_MARK_MAX)
			break;
		mark_inc(mark);
	}
}

static unsigned int node_get_marks(struct xa_node *node, unsigned int offset)
{
	unsigned int marks = 0;
	xa_mark_t mark = XA_MARK_0;

	for (;;) {
		if (node_get_mark(node, offset, mark))
			marks |= 1 << (__force unsigned int)mark;
		if (mark == XA_MARK_MAX)
			break;
		mark_inc(mark);
	}

	return marks;
}

static void node_set_marks(struct xa_node *node, unsigned int offset,
			struct xa_node *child, unsigned int marks)
{
	xa_mark_t mark = XA_MARK_0;

	for (;;) {
		if (marks & (1 << (__force unsigned int)mark)) {
			node_set_mark(node, offset, mark);
			if (child)
				node_mark_all(child, mark);
		}
		if (mark == XA_MARK_MAX)
			break;
		mark_inc(mark);
	}
}

/**
 * xas_split_alloc() - Allocate memory for splitting an entry.
 * @xas: XArray operation state.
 * @entry: New entry which will be stored in the array.
 * @order: Current entry order.
 * @gfp: Memory allocation flags.
 *
 * This function should be called before calling xas_split().
 * If necessary, it will allocate new nodes (and fill them with @entry)
 * to prepare for the upcoming split of an entry of @order size into
 * entries of the order stored in the @xas.
 *
 * Context: May sleep if @gfp flags permit.
 */
void xas_split_alloc(struct xa_state *xas, void *entry, unsigned int order,
		gfp_t gfp)
{
	unsigned int sibs = (1 << (order % XA_CHUNK_SHIFT)) - 1;
	unsigned int mask = xas->xa_sibs;

	/* XXX: no support for splitting really large entries yet */
	if (WARN_ON(xas->xa_shift + 2 * XA_CHUNK_SHIFT < order))
		goto nomem;
	if (xas->xa_shift + XA_CHUNK_SHIFT > order)
		return;

	do {
		unsigned int i;
		void *sibling = NULL;
		struct xa_node *node;

		// node = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
		node = malloc(sizeof(struct xa_node));
		if (!node)
			goto nomem;
		memset(node, 0, sizeof(struct xa_node));
		INIT_LIST_HEAD(&node->private_list);
		node->array = xas->xa;
		for (i = 0; i < XA_CHUNK_SIZE; i++) {
			if ((i & mask) == 0) {
				// RCU_INIT_POINTER(node->slots[i], entry);
				node->slots[i] = entry;
				sibling = xa_mk_sibling(i);
			} else {
				// RCU_INIT_POINTER(node->slots[i], sibling);
				node->slots[i] = sibling;
			}
		}
		// RCU_INIT_POINTER(node->parent, xas->xa_alloc);
		node->parent = xas->xa_alloc;
		xas->xa_alloc = node;
	} while (sibs-- > 0);

	return;
nomem:
	xas_destroy(xas);
	xas_set_err(xas, -ENOMEM);
}

/**
 * xas_split() - Split a multi-index entry into smaller entries.
 * @xas: XArray operation state.
 * @entry: New entry to store in the array.
 * @order: Current entry order.
 *
 * The size of the new entries is set in @xas.  The value in @entry is
 * copied to all the replacement entries.
 *
 * Context: Any context.  The caller should hold the xa_lock.
 */
void xas_split(struct xa_state *xas, void *entry, unsigned int order)
{
	unsigned int sibs = (1 << (order % XA_CHUNK_SHIFT)) - 1;
	unsigned int offset, marks;
	struct xa_node *node;
	void *curr = xas_load(xas);
	int values = 0;

	node = xas->xa_node;
	if (xas_top(node))
		return;

	marks = node_get_marks(node, xas->xa_offset);

	offset = xas->xa_offset + sibs;
	do {
		if (xas->xa_shift < node->shift) {
			struct xa_node *child = xas->xa_alloc;

			// xas->xa_alloc = rcu_dereference_raw(child->parent);
			xas->xa_alloc = child->parent;
			child->shift = node->shift - XA_CHUNK_SHIFT;
			child->offset = offset;
			child->count = XA_CHUNK_SIZE;
			child->nr_values = xa_is_value(entry) ?
					XA_CHUNK_SIZE : 0;
			// RCU_INIT_POINTER(child->parent, node);
			child->parent = node;
			node_set_marks(node, offset, child, marks);
			// rcu_assign_pointer(node->slots[offset], xa_mk_node(child));
			node->slots[offset] = xa_mk_node(child);
			if (xa_is_value(curr))
				values--;
			xas_update(xas, child);
		} else {
			unsigned int canon = offset - xas->xa_sibs;

			node_set_marks(node, canon, NULL, marks);
			// rcu_assign_pointer(node->slots[canon], entry);
			node->slots[canon] = entry;
			while (offset > canon)
				//rcu_assign_pointer(node->slots[offset--],
				//		xa_mk_sibling(canon));
				node->slots[offset--] = xa_mk_sibling(canon);
			values += (xa_is_value(entry) - xa_is_value(curr)) *
					(xas->xa_sibs + 1);
		}
	} while (offset-- > xas->xa_offset);

	node->nr_values += values;
	xas_update(xas, node);
}

/*
 * __xas_prev() - Find the previous entry in the XArray.
 * @xas: XArray operation state.
 *
 * Helper function for xas_prev() which handles all the complex cases
 * out of line.
 */
void *__xas_prev(struct xa_state *xas)
{
	void *entry;

	if (!xas_frozen(xas->xa_node))
		xas->xa_index--;
	if (!xas->xa_node)
		return set_bounds(xas);
	if (xas_not_node(xas->xa_node))
		return xas_load(xas);

	if (xas->xa_offset != get_offset(xas->xa_index, xas->xa_node))
		xas->xa_offset--;

	while (xas->xa_offset == 255) {
		xas->xa_offset = xas->xa_node->offset - 1;
		xas->xa_node = xa_parent(xas->xa, xas->xa_node);
		if (!xas->xa_node)
			return set_bounds(xas);
	}

	for (;;) {
		entry = xa_entry(xas->xa, xas->xa_node, xas->xa_offset);
		if (!xa_is_node(entry))
			return entry;

		xas->xa_node = xa_to_node(entry);
		xas_set_offset(xas);
	}
}

/*
 * __xas_next() - Find the next entry in the XArray.
 * @xas: XArray operation state.
 *
 * Helper function for xas_next() which handles all the complex cases
 * out of line.
 */
void *__xas_next(struct xa_state *xas)
{
	void *entry;

	if (!xas_frozen(xas->xa_node))
		xas->xa_index++;
	if (!xas->xa_node)
		return set_bounds(xas);
	if (xas_not_node(xas->xa_node))
		return xas_load(xas);

	if (xas->xa_offset != get_offset(xas->xa_index, xas->xa_node))
		xas->xa_offset++;

	while (xas->xa_offset == XA_CHUNK_SIZE) {
		xas->xa_offset = xas->xa_node->offset + 1;
		xas->xa_node = xa_parent(xas->xa, xas->xa_node);
		if (!xas->xa_node)
			return set_bounds(xas);
	}

	for (;;) {
		entry = xa_entry(xas->xa, xas->xa_node, xas->xa_offset);
		if (!xa_is_node(entry))
			return entry;

		xas->xa_node = xa_to_node(entry);
		xas_set_offset(xas);
	}
}

/**
 * xas_find() - Find the next present entry in the XArray.
 * @xas: XArray operation state.
 * @max: Highest index to return.
 *
 * If the @xas has not yet been walked to an entry, return the entry
 * which has an index >= xas.xa_index.  If it has been walked, the entry
 * currently being pointed at has been processed, and so we move to the
 * next entry.
 *
 * If no entry is found and the array is smaller than @max, the iterator
 * is set to the smallest index not yet in the array.  This allows @xas
 * to be immediately passed to xas_store().
 *
 * Return: The entry, if found, otherwise %NULL.
 */
void *xas_find(struct xa_state *xas, unsigned long max)
{
	void *entry;

	if (xas_error(xas) || xas->xa_node == XAS_BOUNDS)
		return NULL;
	if (xas->xa_index > max)
		return set_bounds(xas);

	if (!xas->xa_node) {
		xas->xa_index = 1;
		return set_bounds(xas);
	} else if (xas->xa_node == XAS_RESTART) {
		entry = xas_load(xas);
		if (entry || xas_not_node(xas->xa_node))
			return entry;
	} else if (!xas->xa_node->shift &&
		    xas->xa_offset != (xas->xa_index & XA_CHUNK_MASK)) {
		xas->xa_offset = ((xas->xa_index - 1) & XA_CHUNK_MASK) + 1;
	}

	xas_next_offset(xas);

	while (xas->xa_node && (xas->xa_index <= max)) {
		if (unlikely(xas->xa_offset == XA_CHUNK_SIZE)) {
			xas->xa_offset = xas->xa_node->offset + 1;
			xas->xa_node = xa_parent(xas->xa, xas->xa_node);
			continue;
		}

		entry = xa_entry(xas->xa, xas->xa_node, xas->xa_offset);
		if (xa_is_node(entry)) {
			xas->xa_node = xa_to_node(entry);
			xas->xa_offset = 0;
			continue;
		}
		if (entry && !xa_is_sibling(entry))
			return entry;

		xas_next_offset(xas);
	}

	if (!xas->xa_node)
		xas->xa_node = XAS_BOUNDS;
	return NULL;
}

/**
 * xas_find_marked() - Find the next marked entry in the XArray.
 * @xas: XArray operation state.
 * @max: Highest index to return.
 * @mark: Mark number to search for.
 *
 * If the @xas has not yet been walked to an entry, return the marked entry
 * which has an index >= xas.xa_index.  If it has been walked, the entry
 * currently being pointed at has been processed, and so we return the
 * first marked entry with an index > xas.xa_index.
 *
 * If no marked entry is found and the array is smaller than @max, @xas is
 * set to the bounds state and xas->xa_index is set to the smallest index
 * not yet in the array.  This allows @xas to be immediately passed to
 * xas_store().
 *
 * If no entry is found before @max is reached, @xas is set to the restart
 * state.
 *
 * Return: The entry, if found, otherwise %NULL.
 */
void *xas_find_marked(struct xa_state *xas, unsigned long max, xa_mark_t mark)
{
	bool advance = true;
	unsigned int offset;
	void *entry;

	if (xas_error(xas))
		return NULL;
	if (xas->xa_index > max)
		goto max;

	if (!xas->xa_node) {
		xas->xa_index = 1;
		goto out;
	} else if (xas_top(xas->xa_node)) {
		advance = false;
		entry = xa_head(xas->xa);
		xas->xa_node = NULL;
		if (xas->xa_index > max_index(entry))
			goto out;
		if (!xa_is_node(entry)) {
			if (xa_marked(xas->xa, mark))
				return entry;
			xas->xa_index = 1;
			goto out;
		}
		xas->xa_node = xa_to_node(entry);
		xas->xa_offset = xas->xa_index >> xas->xa_node->shift;
	}

	while (xas->xa_index <= max) {
		if (unlikely(xas->xa_offset == XA_CHUNK_SIZE)) {
			xas->xa_offset = xas->xa_node->offset + 1;
			xas->xa_node = xa_parent(xas->xa, xas->xa_node);
			if (!xas->xa_node)
				break;
			advance = false;
			continue;
		}

		if (!advance) {
			entry = xa_entry(xas->xa, xas->xa_node, xas->xa_offset);
			if (xa_is_sibling(entry)) {
				xas->xa_offset = xa_to_sibling(entry);
				xas_move_index(xas, xas->xa_offset);
			}
		}

		offset = xas_find_chunk(xas, advance, mark);
		if (offset > xas->xa_offset) {
			advance = false;
			xas_move_index(xas, offset);
			/* Mind the wrap */
			if ((xas->xa_index - 1) >= max)
				goto max;
			xas->xa_offset = offset;
			if (offset == XA_CHUNK_SIZE)
				continue;
		}

		entry = xa_entry(xas->xa, xas->xa_node, xas->xa_offset);
		if (!entry && !(xa_track_free(xas->xa) && mark == XA_FREE_MARK))
			continue;
		if (!xa_is_node(entry))
			return entry;
		xas->xa_node = xa_to_node(entry);
		xas_set_offset(xas);
	}

out:
	if (xas->xa_index > max)
		goto max;
	return set_bounds(xas);
max:
	xas->xa_node = XAS_RESTART;
	return NULL;
}

/**
 * xas_find_conflict() - Find the next present entry in a range.
 * @xas: XArray operation state.
 *
 * The @xas describes both a range and a position within that range.
 *
 * Context: Any context.  Expects xa_lock to be held.
 * Return: The next entry in the range covered by @xas or %NULL.
 */
void *xas_find_conflict(struct xa_state *xas)
{
	void *curr;

	if (xas_error(xas))
		return NULL;

	if (!xas->xa_node)
		return NULL;

	if (xas_top(xas->xa_node)) {
		curr = xas_start(xas);
		if (!curr)
			return NULL;
		while (xa_is_node(curr)) {
			struct xa_node *node = xa_to_node(curr);
			curr = xas_descend(xas, node);
		}
		if (curr)
			return curr;
	}

	if (xas->xa_node->shift > xas->xa_shift)
		return NULL;

	for (;;) {
		if (xas->xa_node->shift == xas->xa_shift) {
			if ((xas->xa_offset & xas->xa_sibs) == xas->xa_sibs)
				break;
		} else if (xas->xa_offset == XA_CHUNK_MASK) {
			xas->xa_offset = xas->xa_node->offset;
			xas->xa_node = xa_parent_locked(xas->xa, xas->xa_node);
			if (!xas->xa_node)
				break;
			continue;
		}
		curr = xa_entry_locked(xas->xa, xas->xa_node, ++xas->xa_offset);
		if (xa_is_sibling(curr))
			continue;
		while (xa_is_node(curr)) {
			xas->xa_node = xa_to_node(curr);
			xas->xa_offset = 0;
			curr = xa_entry_locked(xas->xa, xas->xa_node, 0);
		}
		if (curr)
			return curr;
	}
	xas->xa_offset -= xas->xa_sibs;
	return NULL;
}

/**
 * xa_load() - Load an entry from an XArray.
 * @xa: XArray.
 * @index: index into array.
 *
 * Context: Any context.  Takes and releases the RCU lock.
 * Return: The entry at @index in @xa.
 */
void *xa_load(struct xarray *xa, unsigned long index)
{
	XA_STATE(xas, xa, index);
	void *entry;

	// rcu_read_lock();
	do {
		entry = xas_load(&xas);
		if (xa_is_zero(entry))
			entry = NULL;
	} while (xas_retry(&xas, entry));
	// rcu_read_unlock();

	return entry;
}

static void *xas_result(struct xa_state *xas, void *curr)
{
	if (xa_is_zero(curr))
		return NULL;
	if (xas_error(xas))
		curr = xas->xa_node;
	return curr;
}

#if 0
/*
 * __xas_nomem() - Drop locks and allocate memory if needed.
 * @xas: XArray operation state.
 * @gfp: Memory allocation flags.
 *
 * Internal variant of xas_nomem().
 *
 * Return: true if memory was needed, and was successfully allocated.
 */
static bool __xas_nomem(struct xa_state *xas, gfp_t gfp)
	//__must_hold(xas->xa->xa_lock)
{
	// unsigned int lock_type = xa_lock_type(xas->xa);

	if (xas->xa_node != XA_ERROR(-ENOMEM)) {
		xas_destroy(xas);
		return false;
	}
	if (xas->xa->xa_flags & XA_FLAGS_ACCOUNT)
		gfp |= __GFP_ACCOUNT;
	/*
	if (gfpflags_allow_blocking(gfp)) {
		xas_unlock_type(xas, lock_type);
		xas->xa_alloc = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
		xas_lock_type(xas, lock_type);
	} else {
		xas->xa_alloc = kmem_cache_alloc_lru(radix_tree_node_cachep, xas->xa_lru, gfp);
	}
	*/
	xas->xa_alloc = malloc();
	if (!xas->xa_alloc)
		return false;
	xas->xa_alloc->parent = NULL;
	// XA_NODE_BUG_ON(xas->xa_alloc, !list_empty(&xas->xa_alloc->private_list));
	xas->xa_node = XAS_RESTART;
	return true;
}
#endif

/**
 * __xa_erase() - Erase this entry from the XArray while locked.
 * @xa: XArray.
 * @index: Index into array.
 *
 * After this function returns, loading from @index will return %NULL.
 * If the index is part of a multi-index entry, all indices will be erased
 * and none of the entries will be part of a multi-index entry.
 *
 * Context: Any context.  Expects xa_lock to be held on entry.
 * Return: The entry which used to be at this index.
 */
void *__xa_erase(struct xarray *xa, unsigned long index)
{
	XA_STATE(xas, xa, index);
	return xas_result(&xas, xas_store(&xas, NULL));
}

/**
 * xa_erase() - Erase this entry from the XArray.
 * @xa: XArray.
 * @index: Index of entry.
 *
 * After this function returns, loading from @index will return %NULL.
 * If the index is part of a multi-index entry, all indices will be erased
 * and none of the entries will be part of a multi-index entry.
 *
 * Context: Any context.  Takes and releases the xa_lock.
 * Return: The entry which used to be at this index.
 */
void *xa_erase(struct xarray *xa, unsigned long index)
{
	void *entry;

	xa_lock(xa);
	entry = __xa_erase(xa, index);
	xa_unlock(xa);

	return entry;
}

/**
 * __xa_store() - Store this entry in the XArray.
 * @xa: XArray.
 * @index: Index into array.
 * @entry: New entry.
 * @gfp: Memory allocation flags.
 *
 * You must already be holding the xa_lock when calling this function.
 * It will drop the lock if needed to allocate memory, and then reacquire
 * it afterwards.
 *
 * Context: Any context.  Expects xa_lock to be held on entry.  May
 * release and reacquire xa_lock if @gfp flags permit.
 * Return: The old entry at this index or xa_err() if an error happened.
 */
void *__xa_store(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	XA_STATE(xas, xa, index);
	void *curr;

	if (WARN_ON_ONCE(xa_is_advanced(entry)))
		return XA_ERROR(-EINVAL);
	if (xa_track_free(xa) && !entry)
		entry = XA_ZERO_ENTRY;

	do {
		curr = xas_store(&xas, entry);
		if (xa_track_free(xa))
			xas_clear_mark(&xas, XA_FREE_MARK);
	} while (__xas_nomem(&xas, gfp));

	return xas_result(&xas, curr);
}

/**
 * xa_store() - Store this entry in the XArray.
 * @xa: XArray.
 * @index: Index into array.
 * @entry: New entry.
 * @gfp: Memory allocation flags.
 *
 * After this function returns, loads from this index will return @entry.
 * Storing into an existing multi-index entry updates the entry of every index.
 * The marks associated with @index are unaffected unless @entry is %NULL.
 *
 * Context: Any context.  Takes and releases the xa_lock.
 * May sleep if the @gfp flags permit.
 * Return: The old entry at this index on success, xa_err(-EINVAL) if @entry
 * cannot be stored in an XArray, or xa_err(-ENOMEM) if memory allocation
 * failed.
 */
void *xa_store(struct xarray *xa, unsigned long index, void *entry, gfp_t gfp)
{
	void *curr;

	xa_lock(xa);
	curr = __xa_store(xa, index, entry, gfp);
	xa_unlock(xa);

	return curr;
}

void xas_set_range(struct xa_state *xas, unsigned long first,
		unsigned long last)
{
	unsigned int shift = 0;
	unsigned long sibs = last - first;
	unsigned int offset = XA_CHUNK_MASK;

	xas_set(xas, first);

	while ((first & XA_CHUNK_MASK) == 0) {
		if (sibs < XA_CHUNK_MASK)
			break;
		if ((sibs == XA_CHUNK_MASK) && (offset < XA_CHUNK_MASK))
			break;
		shift += XA_CHUNK_SHIFT;
		if (offset == XA_CHUNK_MASK)
			offset = sibs & XA_CHUNK_MASK;
		sibs >>= XA_CHUNK_SHIFT;
		first >>= XA_CHUNK_SHIFT;
	}

	offset = first & XA_CHUNK_MASK;
	if (offset + sibs > XA_CHUNK_MASK)
		sibs = XA_CHUNK_MASK - offset;
	if ((((first + sibs + 1) << shift) - 1) > last)
		sibs -= 1;

	xas->xa_shift = shift;
	xas->xa_sibs = sibs;
}

/**
 * xa_store_range() - Store this entry at a range of indices in the XArray.
 * @xa: XArray.
 * @first: First index to affect.
 * @last: Last index to affect.
 * @entry: New entry.
 * @gfp: Memory allocation flags.
 *
 * After this function returns, loads from any index between @first and @last,
 * inclusive will return @entry.
 * Storing into an existing multi-index entry updates the entry of every index.
 * The marks associated with @index are unaffected unless @entry is %NULL.
 *
 * Context: Process context.  Takes and releases the xa_lock.  May sleep
 * if the @gfp flags permit.
 * Return: %NULL on success, xa_err(-EINVAL) if @entry cannot be stored in
 * an XArray, or xa_err(-ENOMEM) if memory allocation failed.
 */
void *xa_store_range(struct xarray *xa, unsigned long first,
		unsigned long last, void *entry, gfp_t gfp)
{
	XA_STATE(xas, xa, 0);

	if (WARN_ON_ONCE(xa_is_internal(entry)))
		return XA_ERROR(-EINVAL);
	if (last < first)
		return XA_ERROR(-EINVAL);

	do {
		xas_lock(&xas);
		if (entry) {
			unsigned int order = BITS_PER_LONG;
			if (last + 1)
				order = __ffs(last + 1);
			xas_set_order(&xas, last, order);
			xas_create(&xas, true);
			if (xas_error(&xas))
				goto unlock;
		}
		do {
			xas_set_range(&xas, first, last);
			xas_store(&xas, entry);
			if (xas_error(&xas))
				goto unlock;
			first += xas_size(&xas);
		} while (first <= last);
unlock:
		xas_unlock(&xas);
	} while (xas_nomem(&xas, gfp));

	return xas_result(&xas, NULL);
}

/**
 * __xa_alloc() - Find somewhere to store this entry in the XArray.
 * @xa: XArray.
 * @id: Pointer to ID.
 * @limit: Range for allocated ID.
 * @entry: New entry.
 * @gfp: Memory allocation flags.
 *
 * Finds an empty entry in @xa between @limit.min and @limit.max,
 * stores the index into the @id pointer, then stores the entry at
 * that index.  A concurrent lookup will not see an uninitialised @id.
 *
 * Context: Any context.  Expects xa_lock to be held on entry.  May
 * release and reacquire xa_lock if @gfp flags permit.
 * Return: 0 on success, -ENOMEM if memory could not be allocated or
 * -EBUSY if there are no free entries in @limit.
 */
int __xa_alloc(struct xarray *xa, u32 *id, void *entry,
		struct xa_limit limit, gfp_t gfp)
{
	XA_STATE(xas, xa, 0);

	if (WARN_ON_ONCE(xa_is_advanced(entry)))
		return -EINVAL;
	if (WARN_ON_ONCE(!xa_track_free(xa)))
		return -EINVAL;

	if (!entry)
		entry = XA_ZERO_ENTRY;

	do {
		xas.xa_index = limit.min;
		xas_find_marked(&xas, limit.max, XA_FREE_MARK);
		if (xas.xa_node == XAS_RESTART)
			xas_set_err(&xas, -EBUSY);
		else
			*id = xas.xa_index;
		xas_store(&xas, entry);
		xas_clear_mark(&xas, XA_FREE_MARK);
	} while (__xas_nomem(&xas, gfp));

	return xas_error(&xas);
}

/**
 * __xa_set_mark() - Set this mark on this entry while locked.
 * @xa: XArray.
 * @index: Index of entry.
 * @mark: Mark number.
 *
 * Attempting to set a mark on a %NULL entry does not succeed.
 *
 * Context: Any context.  Expects xa_lock to be held on entry.
 */
void __xa_set_mark(struct xarray *xa, unsigned long index, xa_mark_t mark)
{
	XA_STATE(xas, xa, index);
	void *entry = xas_load(&xas);

	if (entry)
		xas_set_mark(&xas, mark);
}

/**
 * __xa_clear_mark() - Clear this mark on this entry while locked.
 * @xa: XArray.
 * @index: Index of entry.
 * @mark: Mark number.
 *
 * Context: Any context.  Expects xa_lock to be held on entry.
 */
void __xa_clear_mark(struct xarray *xa, unsigned long index, xa_mark_t mark)
{
	XA_STATE(xas, xa, index);
	void *entry = xas_load(&xas);

	if (entry)
		xas_clear_mark(&xas, mark);
}

/**
 * xa_get_mark() - Inquire whether this mark is set on this entry.
 * @xa: XArray.
 * @index: Index of entry.
 * @mark: Mark number.
 *
 * This function uses the RCU read lock, so the result may be out of date
 * by the time it returns.  If you need the result to be stable, use a lock.
 *
 * Context: Any context.  Takes and releases the RCU lock.
 * Return: True if the entry at @index has this mark set, false if it doesn't.
 */
bool xa_get_mark(struct xarray *xa, unsigned long index, xa_mark_t mark)
{
	XA_STATE(xas, xa, index);
	void *entry;

	//rcu_read_lock();
	entry = xas_start(&xas);
	while (xas_get_mark(&xas, mark)) {
		if (!xa_is_node(entry))
			goto found;
		entry = xas_descend(&xas, xa_to_node(entry));
	}
	//rcu_read_unlock();
	return false;
 found:
	//rcu_read_unlock();
	return true;
}

/**
 * xa_set_mark() - Set this mark on this entry.
 * @xa: XArray.
 * @index: Index of entry.
 * @mark: Mark number.
 *
 * Attempting to set a mark on a %NULL entry does not succeed.
 *
 * Context: Process context.  Takes and releases the xa_lock.
 */
void xa_set_mark(struct xarray *xa, unsigned long index, xa_mark_t mark)
{
	xa_lock(xa);
	__xa_set_mark(xa, index, mark);
	xa_unlock(xa);
}

/**
 * xa_clear_mark() - Clear this mark on this entry.
 * @xa: XArray.
 * @index: Index of entry.
 * @mark: Mark number.
 *
 * Clearing a mark always succeeds.
 *
 * Context: Process context.  Takes and releases the xa_lock.
 */
void xa_clear_mark(struct xarray *xa, unsigned long index, xa_mark_t mark)
{
	xa_lock(xa);
	__xa_clear_mark(xa, index, mark);
	xa_unlock(xa);
}

/**
 * xa_find() - Search the XArray for an entry.
 * @xa: XArray.
 * @indexp: Pointer to an index.
 * @max: Maximum index to search to.
 * @filter: Selection criterion.
 *
 * Finds the entry in @xa which matches the @filter, and has the lowest
 * index that is at least @indexp and no more than @max.
 * If an entry is found, @indexp is updated to be the index of the entry.
 * This function is protected by the RCU read lock, so it may not find
 * entries which are being simultaneously added.  It will not return an
 * %XA_RETRY_ENTRY; if you need to see retry entries, use xas_find().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 * Return: The entry, if found, otherwise %NULL.
 */
void *xa_find(struct xarray *xa, unsigned long *indexp,
			unsigned long max, xa_mark_t filter)
{
	XA_STATE(xas, xa, *indexp);
	void *entry;

	//rcu_read_lock();
	do {
		if ((__force unsigned int)filter < XA_MAX_MARKS)
			entry = xas_find_marked(&xas, max, filter);
		else
			entry = xas_find(&xas, max);
	} while (xas_retry(&xas, entry));
	//rcu_read_unlock();

	if (entry)
		*indexp = xas.xa_index;
	return entry;
}

static bool xas_sibling(struct xa_state *xas)
{
	struct xa_node *node = xas->xa_node;
	unsigned long mask;

	if (!IS_ENABLED(CONFIG_XARRAY_MULTI) || !node)
		return false;
	mask = (XA_CHUNK_SIZE << node->shift) - 1;
	return (xas->xa_index & mask) >
		((unsigned long)xas->xa_offset << node->shift);
}

/**
 * xa_find_after() - Search the XArray for a present entry.
 * @xa: XArray.
 * @indexp: Pointer to an index.
 * @max: Maximum index to search to.
 * @filter: Selection criterion.
 *
 * Finds the entry in @xa which matches the @filter and has the lowest
 * index that is above @indexp and no more than @max.
 * If an entry is found, @indexp is updated to be the index of the entry.
 * This function is protected by the RCU read lock, so it may miss entries
 * which are being simultaneously added.  It will not return an
 * %XA_RETRY_ENTRY; if you need to see retry entries, use xas_find().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 * Return: The pointer, if found, otherwise %NULL.
 */
void *xa_find_after(struct xarray *xa, unsigned long *indexp,
			unsigned long max, xa_mark_t filter)
{
	XA_STATE(xas, xa, *indexp + 1);
	void *entry;

	if (xas.xa_index == 0)
		return NULL;

	//rcu_read_lock();
	for (;;) {
		if ((__force unsigned int)filter < XA_MAX_MARKS)
			entry = xas_find_marked(&xas, max, filter);
		else
			entry = xas_find(&xas, max);

		if (xas_invalid(&xas))
			break;
		if (xas_sibling(&xas))
			continue;
		if (!xas_retry(&xas, entry))
			break;
	}
	//rcu_read_unlock();

	if (entry)
		*indexp = xas.xa_index;
	return entry;
}

/**
 * xa_destroy() - Free all internal data structures.
 * @xa: XArray.
 *
 * After calling this function, the XArray is empty and has freed all memory
 * allocated for its internal data structures.  You are responsible for
 * freeing the objects referenced by the XArray.
 *
 * Context: Any context.  Takes and releases the xa_lock, interrupt-safe.
 */
void xa_destroy(struct xarray *xa)
{
	XA_STATE(xas, xa, 0);
	unsigned long flags;
	void *entry;

	xas.xa_node = NULL;
	xas_lock_irqsave(&xas, flags);
	entry = xa_head_locked(xa);
	// RCU_INIT_POINTER(xa->xa_head, NULL);
	xa->xa_head = NULL;
	xas_init_marks(&xas);
	if (xa_zero_busy(xa))
		xa_mark_clear(xa, XA_FREE_MARK);
	/* lockdep checks we're still holding the lock in xas_free_nodes() */
	if (xa_is_node(entry))
		xas_free_nodes(&xas, xa_to_node(entry));
	xas_unlock_irqrestore(&xas, flags);
}

void xa_dump_node(const struct xa_node *node)
{
	unsigned i, j;

	if (!node)
		return;
	if ((unsigned long)node & 3) {
		printf("node %p\n", node);
		return;
	}

	printf("node %p %s %d parent %p shift %d count %d values %d "
		"array %p list %p %p marks",
		node, node->parent ? "offset" : "max", node->offset,
		node->parent, node->shift, node->count, node->nr_values,
		node->array, node->private_list.prev, node->private_list.next);
	for (i = 0; i < XA_MAX_MARKS; i++)
		for (j = 0; j < XA_MARK_LONGS; j++)
			printf(" %lx", node->marks[i][j]);
	printf("\n");
}

void xa_dump_index(unsigned long index, unsigned int shift, int level)
{
	printf("[%2lu]", (index >> shift) & XA_CHUNK_MASK);
	if (!shift)
		printf("%lu: ", index);
	else if (shift >= BITS_PER_LONG)
		printf("0-%lu: ", ~0UL);
	else
		printf("%lu-%lu: ", index, index | ((1UL << shift) - 1));
}

void xa_dump_entry(const void *entry, unsigned long index,
		unsigned long shift, int level, bool full)
{
	if (!entry) {
		if (full) {
			printf("%*s", level * 4, " ");
			xa_dump_index(index, shift, level);
			printf("%p\n", entry);
		}
		return;
	}

	printf("%*s", level * 4, " ");
	xa_dump_index(index, shift, level);

	if (xa_is_node(entry)) {
		if (shift == 0) {
			printf("%p\n", entry);
		} else {
			unsigned long i;
			struct xa_node *node = xa_to_node(entry);
			xa_dump_node(node);
			for (i = 0; i < XA_CHUNK_SIZE; i++)
				xa_dump_entry(node->slots[i],
				      index + (i << node->shift), node->shift,
				      level + 1, full);
		}
	} else if (xa_is_value(entry))
		printf("value %ld (0x%lx) [%p]\n", xa_to_value(entry),
						xa_to_value(entry), entry);
	else if (!xa_is_internal(entry))
		printf("%p\n", entry);
	else if (xa_is_retry(entry))
		printf("retry (%ld)\n", xa_to_internal(entry));
	else if (xa_is_sibling(entry))
		printf("sibling (slot %ld)\n", xa_to_sibling(entry));
	else if (xa_is_zero(entry))
		printf("zero (%ld)\n", xa_to_internal(entry));
	else
		printf("UNKNOWN ENTRY (%p)\n", entry);
}

void xa_dump(const struct xarray *xa, bool full)
{
	void *entry = xa->xa_head;
	unsigned int shift = 0;

	printf("xarray: %p head %p flags %x marks %d %d %d\n", xa, entry,
			xa->xa_flags, xa_marked(xa, XA_MARK_0),
			xa_marked(xa, XA_MARK_1), xa_marked(xa, XA_MARK_2));
	if (xa_is_node(entry))
		shift = xa_to_node(entry)->shift + XA_CHUNK_SHIFT;
	xa_dump_entry(entry, 0, shift, 0, full);
}
