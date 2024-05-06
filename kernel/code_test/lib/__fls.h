#ifndef _ASM_GENERIC_BITOPS___FLS_H_
#define _ASM_GENERIC_BITOPS___FLS_H_

#include "types.h"

/*
 * 
 * This is defined the same way as ffs.
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
static unsigned long __fls(unsigned long x)
{
	unsigned long r = 64;

	if (!x)
		return 0;
	if (!(x & 0xffffffff00000000u)) {
		x <<= 32;
		r -= 32;
	}
	if (!(x & 0xffff000000000000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff00000000000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf000000000000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc000000000000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x8000000000000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}
#endif /* _ASM_GENERIC_BITOPS___FLS_H_ */
