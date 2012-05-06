/*
 * =====================================================================================
 *
 *       Filename:  resource.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/2012 10:40:27 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <stdio.h>

#define __re_request_resource __re_request_resource2

#define __ALIGN_KERNEL(x, a)		__ALIGN_KERNEL_MASK(x, (typeof(x))(a) - 1)
#define __ALIGN_KERNEL_MASK(x, mask)	(((x) + (mask)) & ~(mask))
#define ALIGN(x, a)		__ALIGN_KERNEL((x), (a))

typedef unsigned long resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    struct resource *parent, *sibling, *child;
};

/* constraints to be met while allocating resources */
struct resource_constraint {
	resource_size_t min, max, align;
	resource_size_t (*alignf)(void *, const struct resource *,
			resource_size_t, resource_size_t);
	void *alignf_data;
};

/* three kinds relation ship between two resources 
 *
 * 1. not overlap
 *                  
 *       +------+          
 *       +------+             
 *                     +-------+
 *                     +-------+
 *                            
 * 2. overlap = ! not overlap
 * 
 *    partial overlap
 *
 *       +------+                  +-------+
 *       +------+                  +-------+
 *           +---------+                   +----------+
 *           +---------+                   +----------+
 *
 *    or contain
 *       partially contain
 *       +-------------------+       +-------------+
 *       +-------------------+       +-------------+
 *                 +------+                +-------+
 *                 +------+                +-------+
 *
 *       the same
 *       +-------------------------+
 *       +-------------------------+
 *
 *       +-------------------------+
 *       +-------------------------+
 */
static inline int resource_not_overlap(struct resource *res1, struct resource *res2)
{
	return (res1->end < res2->start) || (res1->start > res2->end );
}

static inline int resource_overlap(struct resource *res1, struct resource *res2)
{
	/* resource_overlap(res1, res2)
	 * = !resource_not_overlap(res1, res2)
	 * = !( (res1->end < res2->start) || (res1->start > res2->end ) )
	 * = !(res1->end < res2->start) && !(res1->start > res2->end )
	 * = (res1->end >= res2->start) && (res1->start <= res2->end)
	 * */
	return (res1->end >= res2->start) && (res1->start <= res2->end);
}

static inline int res1_contains_res2(struct resource *res1, struct resource *res2)
{
	return (res1->start <= res2->start) && (res1->end >= res2->end);
}

static inline int res1_not_contains_res2(struct resource *res1, struct resource *res2)
{
	/* res1_not_contains_res2(res1, res2)
	 * = !res1_contains_res2(res1, res2)
	 * = !((res1->start <= res2->start) && (res1->end >= res2->end))
	 * = !(res1->start <= res2->start) || !(res1->end >= res2->end)
	 * = (res1->start > res2->start) || (res1->end < res2->end)
	 * */
	return (res1->start > res2->start) || (res1->end < res2->end);
 
}

static inline int resource_contains(struct resource *res1, struct resource *res2)
{
	//return res1_contains_res2(res1, res2) || res1_contains_res2(res2, res1);
	return ((res1->start - res2->start)*(res2->end - res1->end) >= 0);
}

static inline resource_size_t resource_size(const struct resource *res)
{
	return res->end - res->start + 1;
}

void dump(struct resource *root, int level);
int  __re_request_resource1(struct resource *root,
                       struct resource *new, struct resource *old);
int  __re_request_resource2(struct resource *root,
                       struct resource *new, struct resource *old);
int  remove_old(struct resource *root, struct resource *old);
struct resource *insert_resource_conflict(struct resource *parent, struct resource *new);
void release_child_resources(struct resource *r);
int release_resource(struct resource *old);

int allocate_resource(struct resource *root, struct resource *new,
		      resource_size_t size, resource_size_t min,
		      resource_size_t max, resource_size_t align,
		      resource_size_t (*alignf)(void *,
						const struct resource *,
						resource_size_t,
						resource_size_t),
		      void *alignf_data);
resource_size_t simple_align_resource(void *data,
					     const struct resource *avail,
					     resource_size_t size,
					     resource_size_t align);
int find_resource(struct resource *root, struct resource *new,
			resource_size_t size,
			struct resource_constraint  *constraint);
struct resource *request_resource_conflict(struct resource *root, struct resource *new);
int adjust_resource(struct resource *res, resource_size_t start, resource_size_t size);
resource_size_t __find_res_top_free_size(struct resource *res);
void __resource_update_parents_top(struct resource *b_res,
		 long size, struct resource *parent_res);
void __resource_extend_parents_top(struct resource *b_res,
		 long size, struct resource *parent_res);
int extend_res(struct resource *res, resource_size_t size);
#endif //__RESOURCE_H__
