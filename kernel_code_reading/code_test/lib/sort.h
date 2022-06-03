/*
 * =====================================================================================
 *
 *       Filename:  sort.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2013 02:47:02 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __SORT_CASE_
#define __SORT_CASE_

#include <stddef.h>
#include "types.h"

void sort(void *base, size_t num, size_t size,
	  int (*cmp_func)(const void *, const void *),
	  void (*swap_func)(void *, void *, int size));

#endif
