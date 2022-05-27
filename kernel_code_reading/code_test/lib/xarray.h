#ifndef _XARRAY_H
#define _XARRAY_H

#include "bug.h"

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
 * 0-62: Sibling entries
 * 256: Retry entry
 * 257: Zero entry
 *
 * Errors are also represented as internal entries, but use the negative
 * space (-4094 to -2).  They're never stored in the slots array; only
 * returned by the normal API.
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
	WARN_ON((long)v < 0);
	return (void *)((v << 1) | 1);
}

#endif /* _XARRAY_H */
