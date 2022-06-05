#ifndef _XARRAY_H
#define _XARRAY_H
/*
 * eXtensible Arrays
 * Copyright (c) 2017 Microsoft Corporation
 * Author: Matthew Wilcox <willy@infradead.org>
 *
 * See Documentation/core-api/xarray.rst for how to use the XArray.
 */

#include <stdbool.h>
#include "bug.h"
#include "types.h"
#include "list.h"
#include "errno-base.h"
#include "kconfig.h"

/*
 * The bottom two bits of the entry determine how the XArray interprets
 * the contents:
 *
 * 00: Pointer entry
 * 10: Internal entry
 * x1: Value entry or tagged pointer
 *
 * Attempting to store internal entries in the XArray is a bug.
 *
 * Most internal entries are pointers to the next node in the tree.
 * The following internal entries have a special meaning:
 *
 * 0-62: Sibling entries, the possible range is 0-255, only 0-62 is used now
 * 256: Retry entry
 * 257: Zero entry
 *
 * Errors are also represented as internal entries, but use the negative
 * space (-4094 to -2).  They're never stored in the slots array; only
 * returned by the normal API.
 * Question: but xa_is_err() return true for -1.
 *
 * Note: the internal entry's value above, e.g. 256, -2 is the raw value. Not
 * the one encoded.
 */

/* This is because 
 * x1: Value entry
 * So we could only use (BITS_PER_LONG - 1) bits.
 */
#define BITS_PER_XA_VALUE	(BITS_PER_LONG - 1)

/**
 * xa_mk_value() - Create an XArray entry from an integer.
 * @v: Value to store in XArray.
 *
 * Context: Any context.
 * Return: An entry suitable for storing in the XArray.
 */
static inline void *xa_mk_value(unsigned long v)
{
	/* This also because 
	 *   a. Value entry bottom two bits is x1
	 *   b. two complement encoding, (v<0) means highest bit is set
	 */
	WARN_ON((long)v < 0);
	return (void *)((v << 1) | 1);
}

/**
 * xa_to_value() - Get value stored in an XArray entry.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: The value stored in the XArray entry.
 */
static inline unsigned long xa_to_value(const void *entry)
{
	return (unsigned long)entry >> 1;
}

/**
 * xa_is_value() - Determine if an entry is a value.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: True if the entry is a value, false if it is a pointer.
 */
static inline bool xa_is_value(const void *entry)
{
	return (unsigned long)entry & 1;
}

/**
 * xa_tag_pointer() - Create an XArray entry for a tagged pointer.
 * @p: Plain pointer.
 * @tag: Tag value (0, 1 or 3).
 *
 * If the user of the XArray prefers, they can tag their pointers instead
 * of storing value entries.  Three tags are available (0, 1 and 3).
 * These are distinct from the xa_mark_t as they are not replicated up
 * through the array and cannot be searched for.
 *
 * Context: Any context.
 * Return: An XArray entry.
 */
static inline void *xa_tag_pointer(void *p, unsigned long tag)
{
	return (void *)((unsigned long)p | tag);
}

/**
 * xa_untag_pointer() - Turn an XArray entry into a plain pointer.
 * @entry: XArray entry.
 *
 * If you have stored a tagged pointer in the XArray, call this function
 * to get the untagged version of the pointer.
 *
 * Context: Any context.
 * Return: A pointer.
 */
static inline void *xa_untag_pointer(void *entry)
{
	return (void *)((unsigned long)entry & ~3UL);
}

/**
 * xa_pointer_tag() - Get the tag stored in an XArray entry.
 * @entry: XArray entry.
 *
 * If you have stored a tagged pointer in the XArray, call this function
 * to get the tag of that pointer.
 *
 * Context: Any context.
 * Return: A tag.
 */
static inline unsigned int xa_pointer_tag(void *entry)
{
	return (unsigned long)entry & 3UL;
}

/*
 * xa_mk_internal() - Create an internal entry.
 * @v: Value to turn into an internal entry.
 *
 * Internal entries are used for a number of purposes.  Entries 0-255 are
 * used for sibling entries (only 0-62 are used by the current code).  256
 * is used for the retry entry.  257 is used for the reserved / zero entry.
 * Negative internal entries are used to represent errnos.  Node pointers
 * are also tagged as internal entries in some situations.
 *
 * Context: Any context.
 * Return: An XArray internal entry corresponding to this value.
 */
static inline void *xa_mk_internal(unsigned long v)
{
	return (void *)((v << 2) | 2);
}

/*
 * xa_to_internal() - Extract the value from an internal entry.
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: The value which was stored in the internal entry.
 */
static inline unsigned long xa_to_internal(const void *entry)
{
	return (unsigned long)entry >> 2;
}

/*
 * xa_is_internal() - Is the entry an internal entry?
 * @entry: XArray entry.
 *
 * Context: Any context.
 * Return: %true if the entry is an internal entry.
 */
static inline bool xa_is_internal(const void *entry)
{
	return ((unsigned long)entry & 3) == 2;
}

#define XA_ZERO_ENTRY		xa_mk_internal(257)

/**
 * xa_is_zero() - Is the entry a zero entry?
 * @entry: Entry retrieved from the XArray
 *
 * The normal API will return NULL as the contents of a slot containing
 * a zero entry.  You can only see zero entries by using the advanced API.
 *
 * Return: %true if the entry is a zero entry.
 */
static inline bool xa_is_zero(const void *entry)
{
	return unlikely(entry == XA_ZERO_ENTRY);
}

/**
 * xa_is_err() - Report whether an XArray operation returned an error
 * @entry: Result from calling an XArray function
 *
 * If an XArray operation cannot complete an operation, it will return
 * a special value indicating an error.  This function tells you
 * whether an error occurred; xa_err() tells you which error occurred.
 *
 * Context: Any context.
 * Return: %true if the entry indicates an error.
 */
static inline bool xa_is_err(const void *entry)
{
	return unlikely(xa_is_internal(entry) &&
			entry >= xa_mk_internal(-MAX_ERRNO));
}

/**
 * xa_err() - Turn an XArray result into an errno.
 * @entry: Result from calling an XArray function.
 *
 * If an XArray operation cannot complete an operation, it will return
 * a special pointer value which encodes an errno.  This function extracts
 * the errno from the pointer value, or returns 0 if the pointer does not
 * represent an errno.
 *
 * Context: Any context.
 * Return: A negative errno or 0.
 */
static inline int xa_err(void *entry)
{
	/* xa_to_internal() would not do sign extension. */
	if (xa_is_err(entry))
		return (long)entry >> 2;
	return 0;
}

/**
 * struct xa_limit - Represents a range of IDs.
 * @min: The lowest ID to allocate (inclusive).
 * @max: The maximum ID to allocate (inclusive).
 *
 * This structure is used either directly or via the XA_LIMIT() macro
 * to communicate the range of IDs that are valid for allocation.
 * Three common ranges are predefined for you:
 * * xa_limit_32b	- [0 - UINT_MAX]
 * * xa_limit_31b	- [0 - INT_MAX]
 * * xa_limit_16b	- [0 - USHRT_MAX]
 */
struct xa_limit {
	u32 max;
	u32 min;
};

#define XA_LIMIT(_min, _max) (struct xa_limit) { .min = _min, .max = _max }

#define xa_limit_32b	XA_LIMIT(0, UINT_MAX)
#define xa_limit_31b	XA_LIMIT(0, INT_MAX)
#define xa_limit_16b	XA_LIMIT(0, USHRT_MAX)

typedef unsigned xa_mark_t;
#define XA_MARK_0		((__force xa_mark_t)0U)
#define XA_MARK_1		((__force xa_mark_t)1U)
#define XA_MARK_2		((__force xa_mark_t)2U)
#define XA_PRESENT		((__force xa_mark_t)8U)
#define XA_MARK_MAX		XA_MARK_2
#define XA_FREE_MARK		XA_MARK_0

enum xa_lock_type {
	XA_LOCK_IRQ = 1,
	XA_LOCK_BH = 2,
};

/*
 * Values for xa_flags.  The radix tree stores its GFP flags in the xa_flags,
 * and we remain compatible with that.
 */
#define XA_FLAGS_LOCK_IRQ	((__force gfp_t)XA_LOCK_IRQ)
#define XA_FLAGS_LOCK_BH	((__force gfp_t)XA_LOCK_BH)
#define XA_FLAGS_TRACK_FREE	((__force gfp_t)4U)
#define XA_FLAGS_ZERO_BUSY	((__force gfp_t)8U)
#define XA_FLAGS_ALLOC_WRAPPED	((__force gfp_t)16U)
#define XA_FLAGS_ACCOUNT	((__force gfp_t)32U)
#define XA_FLAGS_MARK(mark)	((__force gfp_t)((1U << __GFP_BITS_SHIFT) << \
						(__force unsigned)(mark)))

/* ALLOC is for a normal 0-based alloc.  ALLOC1 is for an 1-based alloc */
#define XA_FLAGS_ALLOC	(XA_FLAGS_TRACK_FREE | XA_FLAGS_MARK(XA_FREE_MARK))
#define XA_FLAGS_ALLOC1	(XA_FLAGS_TRACK_FREE | XA_FLAGS_ZERO_BUSY)

/**
 * struct xarray - The anchor of the XArray.
 * @xa_lock: Lock that protects the contents of the XArray.
 *
 * To use the xarray, define it statically or embed it in your data structure.
 * It is a very small data structure, so it does not usually make sense to
 * allocate it separately and keep a pointer to it in your data structure.
 *
 * You may use the xa_lock to protect your own data structures as well.
 */
/*
 * If all of the entries in the array are NULL, @xa_head is a NULL pointer.
 * If the only non-NULL entry in the array is at index 0, @xa_head is that
 * entry.  If any other entry in the array is non-NULL, @xa_head points
 * to an @xa_node.
 */
struct xarray {
	// spinlock_t	xa_lock;
/* private: The rest of the data structure is not to be used directly. */
	gfp_t		xa_flags;
	void *		xa_head;
};

#define XARRAY_INIT(name, flags) {				\
	.xa_flags = flags,					\
	.xa_head = NULL,					\
}

/**
 * DEFINE_XARRAY_FLAGS() - Define an XArray with custom flags.
 * @name: A string that names your XArray.
 * @flags: XA_FLAG values.
 *
 * This is intended for file scope definitions of XArrays.  It declares
 * and initialises an empty XArray with the chosen name and flags.  It is
 * equivalent to calling xa_init_flags() on the array, but it does the
 * initialisation at compiletime instead of runtime.
 */
#define DEFINE_XARRAY_FLAGS(name, flags)				\
	struct xarray name = XARRAY_INIT(name, flags)

/**
 * DEFINE_XARRAY() - Define an XArray.
 * @name: A string that names your XArray.
 *
 * This is intended for file scope definitions of XArrays.  It declares
 * and initialises an empty XArray with the chosen name.  It is equivalent
 * to calling xa_init() on the array, but it does the initialisation at
 * compiletime instead of runtime.
 */
#define DEFINE_XARRAY(name) DEFINE_XARRAY_FLAGS(name, 0)

/**
 * DEFINE_XARRAY_ALLOC() - Define an XArray which allocates IDs starting at 0.
 * @name: A string that names your XArray.
 *
 * This is intended for file scope definitions of allocating XArrays.
 * See also DEFINE_XARRAY().
 */
#define DEFINE_XARRAY_ALLOC(name) DEFINE_XARRAY_FLAGS(name, XA_FLAGS_ALLOC)

/**
 * DEFINE_XARRAY_ALLOC1() - Define an XArray which allocates IDs starting at 1.
 * @name: A string that names your XArray.
 *
 * This is intended for file scope definitions of allocating XArrays.
 * See also DEFINE_XARRAY().
 */
#define DEFINE_XARRAY_ALLOC1(name) DEFINE_XARRAY_FLAGS(name, XA_FLAGS_ALLOC1)

void *xa_load(struct xarray *, unsigned long index);
void *xa_store(struct xarray *, unsigned long index, void *entry, gfp_t);
void *xa_erase(struct xarray *, unsigned long index);
void *xa_store_range(struct xarray *, unsigned long first, unsigned long last,
			void *entry, gfp_t);
bool xa_get_mark(struct xarray *, unsigned long index, xa_mark_t);
void xa_set_mark(struct xarray *, unsigned long index, xa_mark_t);
void xa_clear_mark(struct xarray *, unsigned long index, xa_mark_t);
void *xa_find(struct xarray *xa, unsigned long *index,
		unsigned long max, xa_mark_t);
void *xa_find_after(struct xarray *xa, unsigned long *index,
		unsigned long max, xa_mark_t);
unsigned int xa_extract(struct xarray *, void **dst, unsigned long start,
		unsigned long max, unsigned int n, xa_mark_t);
void xa_destroy(struct xarray *);

/**
 * xa_init_flags() - Initialise an empty XArray with flags.
 * @xa: XArray.
 * @flags: XA_FLAG values.
 *
 * If you need to initialise an XArray with special flags (eg you need
 * to take the lock from interrupt context), use this function instead
 * of xa_init().
 *
 * Context: Any context.
 */
static inline void xa_init_flags(struct xarray *xa, gfp_t flags)
{
	// spin_lock_init(&xa->xa_lock);
	xa->xa_flags = flags;
	xa->xa_head = NULL;
}

/**
 * xa_init() - Initialise an empty XArray.
 * @xa: XArray.
 *
 * An empty XArray is full of NULL entries.
 *
 * Context: Any context.
 */
static inline void xa_init(struct xarray *xa)
{
	xa_init_flags(xa, 0);
}

/**
 * xa_empty() - Determine if an array has any present entries.
 * @xa: XArray.
 *
 * Context: Any context.
 * Return: %true if the array contains only NULL pointers.
 */
static inline bool xa_empty(const struct xarray *xa)
{
	return xa->xa_head == NULL;
}

/**
 * xa_marked() - Inquire whether any entry in this array has a mark set
 * @xa: Array
 * @mark: Mark value
 *
 * Context: Any context.
 * Return: %true if any entry has this mark set.
 */
static inline bool xa_marked(const struct xarray *xa, xa_mark_t mark)
{
	return xa->xa_flags & XA_FLAGS_MARK(mark);
}

/**
 * xa_for_each_range() - Iterate over a portion of an XArray.
 * @xa: XArray.
 * @index: Index of @entry.
 * @entry: Entry retrieved from array.
 * @start: First index to retrieve from array.
 * @last: Last index to retrieve from array.
 *
 * During the iteration, @entry will have the value of the entry stored
 * in @xa at @index.  You may modify @index during the iteration if you
 * want to skip or reprocess indices.  It is safe to modify the array
 * during the iteration.  At the end of the iteration, @entry will be set
 * to NULL and @index will have a value less than or equal to max.
 *
 * xa_for_each_range() is O(n.log(n)) while xas_for_each() is O(n).  You have
 * to handle your own locking with xas_for_each(), and if you have to unlock
 * after each iteration, it will also end up being O(n.log(n)).
 * xa_for_each_range() will spin if it hits a retry entry; if you intend to
 * see retry entries, you should use the xas_for_each() iterator instead.
 * The xas_for_each() iterator will expand into more inline code than
 * xa_for_each_range().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 */
#define xa_for_each_range(xa, index, entry, start, last)		\
	for (index = start,						\
	     entry = xa_find(xa, &index, last, XA_PRESENT);		\
	     entry;							\
	     entry = xa_find_after(xa, &index, last, XA_PRESENT))

/**
 * xa_for_each_start() - Iterate over a portion of an XArray.
 * @xa: XArray.
 * @index: Index of @entry.
 * @entry: Entry retrieved from array.
 * @start: First index to retrieve from array.
 *
 * During the iteration, @entry will have the value of the entry stored
 * in @xa at @index.  You may modify @index during the iteration if you
 * want to skip or reprocess indices.  It is safe to modify the array
 * during the iteration.  At the end of the iteration, @entry will be set
 * to NULL and @index will have a value less than or equal to max.
 *
 * xa_for_each_start() is O(n.log(n)) while xas_for_each() is O(n).  You have
 * to handle your own locking with xas_for_each(), and if you have to unlock
 * after each iteration, it will also end up being O(n.log(n)).
 * xa_for_each_start() will spin if it hits a retry entry; if you intend to
 * see retry entries, you should use the xas_for_each() iterator instead.
 * The xas_for_each() iterator will expand into more inline code than
 * xa_for_each_start().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 */
#define xa_for_each_start(xa, index, entry, start) \
	xa_for_each_range(xa, index, entry, start, ULONG_MAX)

/**
 * xa_for_each() - Iterate over present entries in an XArray.
 * @xa: XArray.
 * @index: Index of @entry.
 * @entry: Entry retrieved from array.
 *
 * During the iteration, @entry will have the value of the entry stored
 * in @xa at @index.  You may modify @index during the iteration if you want
 * to skip or reprocess indices.  It is safe to modify the array during the
 * iteration.  At the end of the iteration, @entry will be set to NULL and
 * @index will have a value less than or equal to max.
 *
 * xa_for_each() is O(n.log(n)) while xas_for_each() is O(n).  You have
 * to handle your own locking with xas_for_each(), and if you have to unlock
 * after each iteration, it will also end up being O(n.log(n)).  xa_for_each()
 * will spin if it hits a retry entry; if you intend to see retry entries,
 * you should use the xas_for_each() iterator instead.  The xas_for_each()
 * iterator will expand into more inline code than xa_for_each().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 */
#define xa_for_each(xa, index, entry) \
	xa_for_each_start(xa, index, entry, 0)

/**
 * xa_for_each_marked() - Iterate over marked entries in an XArray.
 * @xa: XArray.
 * @index: Index of @entry.
 * @entry: Entry retrieved from array.
 * @filter: Selection criterion.
 *
 * During the iteration, @entry will have the value of the entry stored
 * in @xa at @index.  The iteration will skip all entries in the array
 * which do not match @filter.  You may modify @index during the iteration
 * if you want to skip or reprocess indices.  It is safe to modify the array
 * during the iteration.  At the end of the iteration, @entry will be set to
 * NULL and @index will have a value less than or equal to max.
 *
 * xa_for_each_marked() is O(n.log(n)) while xas_for_each_marked() is O(n).
 * You have to handle your own locking with xas_for_each(), and if you have
 * to unlock after each iteration, it will also end up being O(n.log(n)).
 * xa_for_each_marked() will spin if it hits a retry entry; if you intend to
 * see retry entries, you should use the xas_for_each_marked() iterator
 * instead.  The xas_for_each_marked() iterator will expand into more inline
 * code than xa_for_each_marked().
 *
 * Context: Any context.  Takes and releases the RCU lock.
 */
#define xa_for_each_marked(xa, index, entry, filter) \
	for (index = 0, entry = xa_find(xa, &index, ULONG_MAX, filter); \
	     entry; entry = xa_find_after(xa, &index, ULONG_MAX, filter))

#define xa_trylock(xa)		// spin_trylock(&(xa)->xa_lock)
#define xa_lock(xa)		// spin_lock(&(xa)->xa_lock)
#define xa_unlock(xa)		// spin_unlock(&(xa)->xa_lock)
#define xa_lock_bh(xa)		// spin_lock_bh(&(xa)->xa_lock)
#define xa_unlock_bh(xa)	// spin_unlock_bh(&(xa)->xa_lock)
#define xa_lock_irq(xa)		// spin_lock_irq(&(xa)->xa_lock)
#define xa_unlock_irq(xa)	// spin_unlock_irq(&(xa)->xa_lock)
#define xa_lock_irqsave(xa, flags) \
				// spin_lock_irqsave(&(xa)->xa_lock, flags)
#define xa_unlock_irqrestore(xa, flags) \
				// spin_unlock_irqrestore(&(xa)->xa_lock, flags)
#define xa_lock_nested(xa, subclass) \
				// spin_lock_nested(&(xa)->xa_lock, subclass)
#define xa_lock_bh_nested(xa, subclass) \
				// spin_lock_bh_nested(&(xa)->xa_lock, subclass)
#define xa_lock_irq_nested(xa, subclass) \
				// spin_lock_irq_nested(&(xa)->xa_lock, subclass)
#define xa_lock_irqsave_nested(xa, flags, subclass) \
		// spin_lock_irqsave_nested(&(xa)->xa_lock, flags, subclass)

/*
 * Versions of the normal API which require the caller to hold the
 * xa_lock.  If the GFP flags allow it, they will drop the lock to
 * allocate memory, then reacquire it afterwards.  These functions
 * may also re-enable interrupts if the XArray flags indicate the
 * locking should be interrupt safe.
 */
void *__xa_erase(struct xarray *, unsigned long index);
void *__xa_store(struct xarray *, unsigned long index, void *entry, gfp_t);
void *__xa_cmpxchg(struct xarray *, unsigned long index, void *old,
		void *entry, gfp_t);
int __xa_insert(struct xarray *, unsigned long index,
		void *entry, gfp_t);
int __xa_alloc(struct xarray *, u32 *id, void *entry,
		struct xa_limit, gfp_t);
int __xa_alloc_cyclic(struct xarray *, u32 *id, void *entry,
		struct xa_limit, u32 *next, gfp_t);
void __xa_set_mark(struct xarray *, unsigned long index, xa_mark_t);
void __xa_clear_mark(struct xarray *, unsigned long index, xa_mark_t);

/* Everything below here is the Advanced API.  Proceed with caution. */

/*
 * The xarray is constructed out of a set of 'chunks' of pointers.  Choosing
 * the best chunk size requires some tradeoffs.  A power of two recommends
 * itself so that we can walk the tree based purely on shifts and masks.
 * Generally, the larger the better; as the number of slots per level of the
 * tree increases, the less tall the tree needs to be.  But that needs to be
 * balanced against the memory consumption of each node.  On a 64-bit system,
 * xa_node is currently 576 bytes, and we get 7 of them per 4kB page.  If we
 * doubled the number of slots per node, we'd get only 3 nodes per 4kB page.
 */
#ifndef XA_CHUNK_SHIFT
#define XA_CHUNK_SHIFT		(4)
#endif
#define XA_CHUNK_SIZE		(1UL << XA_CHUNK_SHIFT)
#define XA_CHUNK_MASK		(XA_CHUNK_SIZE - 1)
#define XA_MAX_MARKS		3
#define XA_MARK_LONGS		DIV_ROUND_UP(XA_CHUNK_SIZE, BITS_PER_LONG)

/*
 * @count is the count of every non-NULL element in the ->slots array
 * whether that is a value entry, a retry entry, a user pointer,
 * a sibling entry or a pointer to the next level of the tree.
 * @nr_values is the count of every element in ->slots which is
 * either a value entry or a sibling of a value entry.
 */
struct xa_node {
	unsigned char	shift;		/* Bits remaining in each slot */
	unsigned char	offset;		/* Slot offset in parent */
	unsigned char	count;		/* Total entry count */
	unsigned char	nr_values;	/* Value entry count */
	struct xa_node *parent;		/* NULL at top of tree */
	struct xarray	*array;		/* The array we belong to */
	union {
		struct list_head private_list;	/* For tree user */
		struct rcu_head	rcu_head;	/* Used when freeing node */
	};
	void 		*slots[XA_CHUNK_SIZE];
	union {
		unsigned long	tags[XA_MAX_MARKS][XA_MARK_LONGS];
		unsigned long	marks[XA_MAX_MARKS][XA_MARK_LONGS];
	};
};

void xa_dump(const struct xarray *);
void xa_dump_node(const struct xa_node *);

#ifdef XA_DEBUG
#define XA_BUG_ON(xa, x) do {					\
		if (x) {					\
			xa_dump(xa);				\
			BUG();					\
		}						\
	} while (0)
#define XA_NODE_BUG_ON(node, x) do {				\
		if (x) {					\
			if (node) xa_dump_node(node);		\
			BUG();					\
		}						\
	} while (0)
#else
#define XA_BUG_ON(xa, x)	do { } while (0)
#define XA_NODE_BUG_ON(node, x)	do { } while (0)
#endif

/* Private */
static inline void *xa_head(const struct xarray *xa)
{
	// return rcu_dereference_check(xa->xa_head, lockdep_is_held(&xa->xa_lock));
	return xa->xa_head;
}

/* Private */
static inline void *xa_head_locked(const struct xarray *xa)
{
	// return rcu_dereference_protected(xa->xa_head, lockdep_is_held(&xa->xa_lock));
	return xa->xa_head;
}

/* Private */
static inline void *xa_entry(const struct xarray *xa,
				const struct xa_node *node, unsigned int offset)
{
	XA_NODE_BUG_ON(node, offset >= XA_CHUNK_SIZE);
	// return rcu_dereference_check(node->slots[offset], lockdep_is_held(&xa->xa_lock));
	return node->slots[offset];
}

/* Private */
static inline void *xa_entry_locked(const struct xarray *xa,
				const struct xa_node *node, unsigned int offset)
{
	XA_NODE_BUG_ON(node, offset >= XA_CHUNK_SIZE);
	// return rcu_dereference_protected(node->slots[offset], lockdep_is_held(&xa->xa_lock));
	return node->slots[offset];
}

/* Private */
static inline struct xa_node *xa_parent(const struct xarray *xa,
					const struct xa_node *node)
{
	// return rcu_dereference_check(node->parent, lockdep_is_held(&xa->xa_lock));
	return node->parent;
}

/* Private */
static inline struct xa_node *xa_parent_locked(const struct xarray *xa,
					const struct xa_node *node)
{
	// return rcu_dereference_protected(node->parent, lockdep_is_held(&xa->xa_lock));
	return node->parent;
}

/* Private */
static inline void *xa_mk_node(const struct xa_node *node)
{
	return (void *)((unsigned long)node | 2);
}

/* Private */
static inline struct xa_node *xa_to_node(const void *entry)
{
	return (struct xa_node *)((unsigned long)entry - 2);
}

/* Private */
static inline bool xa_is_node(const void *entry)
{
	// why entry must bigger than 4096?
	return xa_is_internal(entry) && (unsigned long)entry > 4096;
}

/* Private */
static inline void *xa_mk_sibling(unsigned int offset)
{
	return xa_mk_internal(offset);
}

/* Private */
static inline unsigned long xa_to_sibling(const void *entry)
{
	return xa_to_internal(entry);
}

/**
 * xa_is_sibling() - Is the entry a sibling entry?
 * @entry: Entry retrieved from the XArray
 *
 * Return: %true if the entry is a sibling entry.
 */
static inline bool xa_is_sibling(const void *entry)
{
	return IS_ENABLED(CONFIG_XARRAY_MULTI) && xa_is_internal(entry) &&
		(entry < xa_mk_sibling(XA_CHUNK_SIZE - 1));
}

#define XA_RETRY_ENTRY		xa_mk_internal(256)

/**
 * xa_is_retry() - Is the entry a retry entry?
 * @entry: Entry retrieved from the XArray
 *
 * Return: %true if the entry is a retry entry.
 */
static inline bool xa_is_retry(const void *entry)
{
	return unlikely(entry == XA_RETRY_ENTRY);
}

/**
 * xa_is_advanced() - Is the entry only permitted for the advanced API?
 * @entry: Entry to be stored in the XArray.
 *
 * Return: %true if the entry cannot be stored by the normal API.
 */
static inline bool xa_is_advanced(const void *entry)
{
	return xa_is_internal(entry) && (entry <= XA_RETRY_ENTRY);
}

/**
 * typedef xa_update_node_t - A callback function from the XArray.
 * @node: The node which is being processed
 *
 * This function is called every time the XArray updates the count of
 * present and value entries in a node.  It allows advanced users to
 * maintain the private_list in the node.
 *
 * Context: The xa_lock is held and interrupts may be disabled.
 *	    Implementations should not drop the xa_lock, nor re-enable
 *	    interrupts.
 */
typedef void (*xa_update_node_t)(struct xa_node *node);

/*
 * The xa_state is opaque to its users.  It contains various different pieces
 * of state involved in the current operation on the XArray.  It should be
 * declared on the stack and passed between the various internal routines.
 * The various elements in it should not be accessed directly, but only
 * through the provided accessor functions.  The below documentation is for
 * the benefit of those working on the code, not for users of the XArray.
 *
 * @xa_node usually points to the xa_node containing the slot we're operating
 * on (and @xa_offset is the offset in the slots array).  If there is a
 * single entry in the array at index 0, there are no allocated xa_nodes to
 * point to, and so we store %NULL in @xa_node.  @xa_node is set to
 * the value %XAS_RESTART if the xa_state is not walked to the correct
 * position in the tree of nodes for this operation.  If an error occurs
 * during an operation, it is set to an %XAS_ERROR value.  If we run off the
 * end of the allocated nodes, it is set to %XAS_BOUNDS.
 */
struct xa_state {
	struct xarray *xa;
	unsigned long xa_index;
	unsigned char xa_shift;
	unsigned char xa_sibs;
	unsigned char xa_offset;
	unsigned char xa_pad;		/* Helps gcc generate better code */
	struct xa_node *xa_node;
	struct xa_node *xa_alloc;
	xa_update_node_t xa_update;
	struct list_lru *xa_lru;
};

/*
 * We encode errnos in the xas->xa_node.  If an error has happened, we need to
 * drop the lock to fix it, and once we've done so the xa_state is invalid.
 */
#define XA_ERROR(errno) ((struct xa_node *)(((unsigned long)errno << 2) | 2UL))
#define XAS_BOUNDS	((struct xa_node *)1UL)
#define XAS_RESTART	((struct xa_node *)3UL)

#define __XA_STATE(array, index, shift, sibs)  {	\
	.xa = array,					\
	.xa_index = index,				\
	.xa_shift = shift,				\
	.xa_sibs = sibs,				\
	.xa_offset = 0,					\
	.xa_pad = 0,					\
	.xa_node = XAS_RESTART,				\
	.xa_alloc = NULL,				\
	.xa_update = NULL,				\
	.xa_lru = NULL,					\
}

/**
 * XA_STATE() - Declare an XArray operation state.
 * @name: Name of this operation state (usually xas).
 * @array: Array to operate on.
 * @index: Initial index of interest.
 *
 * Declare and initialise an xa_state on the stack.
 */
#define XA_STATE(name, array, index)				\
	struct xa_state name = __XA_STATE(array, index, 0, 0)
#endif /* _XARRAY_H */
