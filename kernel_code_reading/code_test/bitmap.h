/*
 * =====================================================================================
 *
 *       Filename:  bitmap.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/28/2013 11:41:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

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
