/*
 * =====================================================================================
 *
 *       Filename:  idr.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/20/2012 10:27:48 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _IDR_H_
#define _IDR_H_

#include "types.h"

# define IDR_BITS 5

struct idr_layer {
	unsigned long		 bitmap; /* A zero bit means "space here" */
	struct idr_layer        *ary[1<<IDR_BITS];
	int			 count;	 /* When zero, we can release it */
	int			 layer;	 /* distance from leaf */
	struct rcu_head		 rcu_head;
};

struct idr {
	struct idr_layer *top;
	struct idr_layer *id_free;
	int		  layers; /* only valid without concurrent changes */
	int		  id_free_cnt;
	//spinlock_t	  lock;
};

void idr_init(struct idr *idp);
#endif
