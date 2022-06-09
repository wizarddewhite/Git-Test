/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_FIND_H_
#define __LINUX_FIND_H_

#define __AC(X,Y)	(X##Y)
#define _AC(X,Y)	__AC(X,Y)
#define _UL(x)		(_AC(x, UL))
#define UL(x)		(_UL(x))

#define GENMASK_INPUT_CHECK(h, l) 0

#define __GENMASK(h, l) \
	(((~UL(0)) - (UL(1) << (l)) + 1) & \
	 (~UL(0) >> (BITS_PER_LONG - 1 - (h))))
#define GENMASK(h, l) \
	(GENMASK_INPUT_CHECK(h, l) + __GENMASK(h, l))

/**
 * find_next_bit - find the next set bit in a memory region
 * @addr: The address to base the search on
 * @offset: The bitnumber to start searching at
 * @size: The bitmap size in bits
 *
 * Returns the bit number for the next set bit
 * If no bits are set, returns @size.
 */
static inline
unsigned long find_next_bit(const unsigned long *addr, unsigned long size,
			    unsigned long offset)
{
	// if (small_const_nbits(size)) {
	if (size <= BITS_PER_LONG && size > 0) {
		unsigned long val;

		if (unlikely(offset >= size))
			return size;

		val = *addr & GENMASK(size - 1, offset);
		return val ? __ffs(val) : size;
	}

	// return _find_next_bit(addr, NULL, size, offset, 0UL, 0);
	return 0;
}
#endif  // __LINUX_FIND_H_
