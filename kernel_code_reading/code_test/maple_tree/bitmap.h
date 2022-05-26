#ifndef __LINUX_BITMAP_H
#define __LINUX_BITMAP_H

#include <string.h>
#include "types.h"

static inline void bitmap_zero(unsigned long *dst, int nbits)
{
	int len = BITS_TO_LONGS(nbits) * sizeof(unsigned long);
	memset(dst, 0, len);
}

#endif
