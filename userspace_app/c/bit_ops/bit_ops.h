#ifndef _BIT_OPS_
#define _BIT_OPS_

#define BITS_PER_BYTE           8
#define BITS_PER_LONG           (sizeof (unsigned long) * BITS_PER_BYTE)

#define BITMAP_FIRST_WORD_MASK(start) (~0UL << ((start) & (BITS_PER_LONG - 1)))
#define BITMAP_LAST_WORD_MASK(nbits) (~0UL >> (-(nbits) & (BITS_PER_LONG - 1)))

#endif
