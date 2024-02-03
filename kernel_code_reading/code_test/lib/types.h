/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/20/2012 10:32:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include "bitops.h"

#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

/**
 * struct callback_head - callback structure for use with RCU and task_work
 * @next: next update requests in a list
 * @func: actual update function to call after the grace period.
 *
 * The struct is aligned to size of pointer. On most architectures it happens
 * naturally due ABI requirements, but some architectures (like CRIS) have
 * weird ABI and we need to ask it explicitly.
 *
 * The alignment is required to guarantee that bit 0 of @next will be
 * clear under normal conditions -- as long as we use call_rcu() or
 * call_srcu() to queue the callback.
 *
 * This guarantee is important for few reasons:
 *  - future call_rcu_lazy() will make use of lower bits in the pointer;
 *  - the structure shares storage space in struct page with @compound_head,
 *    which encode PageTail() in bit 0. The guarantee is needed to avoid
 *    false-positive PageTail().
 */
struct callback_head {
	struct callback_head *next;
	void (*func)(struct callback_head *head);
};
#define rcu_head callback_head

typedef unsigned int gfp_t;
typedef unsigned int u32;

#endif
