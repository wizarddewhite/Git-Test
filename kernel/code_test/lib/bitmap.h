#ifndef __LINUX_BITMAP_H
#define __LINUX_BITMAP_H

#include <string.h>
#include <stdbool.h>
#include "types.h"
#include "find.h"

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

static inline void bitmap_zero(unsigned long *dst, int nbits)
{
	int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
	memset(dst, 0, len);
}

static inline void bitmap_fill(unsigned long *dst, unsigned int nbits)
{
	unsigned int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
	memset(dst, 0xff, len);
}

static inline bool bitmap_empty(const unsigned long *src, unsigned nbits)
{
	return find_first_bit(src, nbits) == nbits;
}

void __bitmap_clear(unsigned long *map, unsigned int start, int len);
static void bitmap_clear(unsigned long *map, unsigned int start,
		unsigned int nbits)
{
	// if (__builtin_constant_p(nbits) && nbits == 1)
	// 	__clear_bit(start, map);
	// else if (__builtin_constant_p(start & BITMAP_MEM_MASK) &&
	// 	 IS_ALIGNED(start, BITMAP_MEM_ALIGNMENT) &&
	// 	 __builtin_constant_p(nbits & BITMAP_MEM_MASK) &&
	// 	 IS_ALIGNED(nbits, BITMAP_MEM_ALIGNMENT))
	// 	memset((char *)map + start / 8, 0, nbits / 8);
	// else
		__bitmap_clear(map, start, nbits);
}

#endif
