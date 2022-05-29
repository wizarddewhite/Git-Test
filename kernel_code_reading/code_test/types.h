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
 */
struct callback_head {
	struct callback_head *next;
	void (*func)(struct callback_head *head);
};
#define rcu_head callback_head

typedef unsigned int gfp_t;
#endif
