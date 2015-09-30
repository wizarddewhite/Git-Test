/*
 * =====================================================================================
 *
 *       Filename:  slub.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/29/2015 09:12:04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include "slub.h"

#define MAX_ORDER 11
#define MAX_OBJS_PER_PAGE	32767 /* since page.objects is u15 */

int get_order(unsigned long size)
{
	int order;

	size--;
	size >>= PAGE_SHIFT;
	order = fls(size);
	return order;
}

/*
 * PAGE_ALLOC_COSTLY_ORDER is the order at which allocations are deemed
 * costly to service.  That is between allocation orders which should
 * coalesce naturally under reasonable reclaim pressure and those which
 * will not.
 */
#define PAGE_ALLOC_COSTLY_ORDER 3

/*
 * Mininum / Maximum order of slab pages. This influences locking overhead
 * and slab fragmentation. A higher order reduces the number of partial slabs
 * and increases the number of allocations possible without having to
 * take the list_lock.
 */
int slub_min_order;
int slub_max_order = PAGE_ALLOC_COSTLY_ORDER;
int slub_min_objects;

int nr_cpu_ids = 4;

static inline int order_objects(int order, unsigned long size, int reserved)
{
	return ((PAGE_SIZE << order) - reserved) / size;
}

/*
 * Calculate the order of allocation given an slab object size.
 *
 * The order of allocation has significant impact on performance and other
 * system components. Generally order 0 allocations should be preferred since
 * order 0 does not cause fragmentation in the page allocator. Larger objects
 * be problematic to put into order 0 slabs because there may be too much
 * unused space left. We go to a higher order if more than 1/16th of the slab
 * would be wasted.
 *
 * In order to reach satisfactory performance we must ensure that a minimum
 * number of objects is in one slab. Otherwise we may generate too much
 * activity on the partial lists which requires taking the list_lock. This is
 * less a concern for large slabs though which are rarely used.
 *
 * slub_max_order specifies the order where we begin to stop considering the
 * number of objects in a slab as critical. If we reach slub_max_order then
 * we try to keep the page order as low as possible. So we accept more waste
 * of space in favor of a small page order.
 *
 * Higher order allocations also allow the placement of more objects in a
 * slab and thereby reduce object handling overhead. If the user has
 * requested a higher mininum order then we start with that one instead of
 * the smallest order which will fit the object.
 */
static inline int slab_order(int size, int min_objects,
				int max_order, int fract_leftover, int reserved)
{
	int order;
	int rem;
	int min_order = slub_min_order;

	if (order_objects(min_order, size, reserved) > MAX_OBJS_PER_PAGE)
		return get_order(size * MAX_OBJS_PER_PAGE) - 1;

	printf("\t -- min_order:%d, obj_sized order:%d\n",
		min_order, get_order(min_objects * size));
	for (order = max(min_order, get_order(min_objects * size));
			order <= max_order; order++) {

		unsigned long slab_size = PAGE_SIZE << order;

		printf("\t   order:%d ", order);

		if (slab_size < min_objects * size + reserved) {
			printf(", slab_size(0x%lx) < min_objects * size + reserved(0x%x)\n",
					slab_size,  min_objects * size + reserved);
			continue;
		}

		rem = (slab_size - reserved) % size;

		printf(", waste:%d(%f)\n", rem, (double)rem/slab_size);
		if (rem <= slab_size / fract_leftover)
			break;

	}

	return order;
}

int calculate_order(int size, int reserved)
{
	int order;
	int min_objects;
	int fraction;
	int max_objects;

	/*
	 * Attempt to find best configuration for a slab. This
	 * works by first attempting to generate a layout with
	 * the best configuration and backing off gradually.
	 *
	 * First we reduce the acceptable waste in a slab. Then
	 * we reduce the minimum objects required in a slab.
	 */
	min_objects = slub_min_objects;
	if (!min_objects)
		min_objects = 4 * (fls(nr_cpu_ids) + 1);
	max_objects = order_objects(slub_max_order, size, reserved);
	printf(" --- min_objects %d, max_objects %d, slub_max_order %d\n",
		     min_objects, max_objects, slub_max_order);

	min_objects = min(min_objects, max_objects);
	printf("     smaller one is %d\n\n", min_objects);

	while (min_objects > 1) {
		fraction = 16;
		while (fraction >= 4) {
			printf("    --- Try with %d min_objects 1/%d waste ---\n",
				min_objects, fraction);

			order = slab_order(size, min_objects,
					slub_max_order, fraction, reserved);
			if (order <= slub_max_order)
				return order;
			fraction /= 2;
		}
		min_objects--;
	}

	printf("    --- Try with min_objects(1) max_order(%d) and tolerant total waste ---\n", slub_max_order);
	/*
	 * We were unable to place multiple objects in a slab. Now
	 * lets see if we can place a single object there.
	 */
	order = slab_order(size, 1, slub_max_order, 1, reserved);
	if (order <= slub_max_order)
		return order;

	printf("    --- Try with min_objects(1) max_order(%d) and tolerant total waste ---\n", MAX_ORDER);
	/*
	 * Doh this slab cannot be placed using slub_max_order.
	 */
	order = slab_order(size, 1, MAX_ORDER, 1, reserved);
	if (order < MAX_ORDER)
		return order;
	return -12;
}
