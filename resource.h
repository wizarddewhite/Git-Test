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

typedef unsigned long resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    struct resource *parent, *sibling, *child;
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
	return !resource_not_overlap(res1, res2);
}

static inline int res1_contains_res2(struct resource *res1, struct resource *res2)
{
	return (res1->start <= res2->start) && (res1->end >= res2->end);
}

static inline int resource_contains(struct resource *res1, struct resource *res2)
{
	return res1_contains_res2(res1, res2) || res1_contains_res2(res2, res1);
}
