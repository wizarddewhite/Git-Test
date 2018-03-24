/*
 * =====================================================================================
 *
 *       Filename:  resource.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/13/2012 02:50:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "resource.h"
#include "resource_case.h"

struct resource iomem_resource = {
	.name	= "PCI mem",
	.start	= 0,
	.end	= -1,
	.flags	= IORESOURCE_MEM,
};

void dump(struct resource *root, int level)
{
	if (!root)
		return;

	/* print itself first */
	printf("%*c%lu-%lu  %s\n",level*3, ' ', root->start, root->end,
			root->flags|IORESOURCE_BUSY?"busy":"none");

	/* depth first */
	dump(root->child, level+1);

	/* then brothers */
	dump(root->sibling, level);

	return;
}

void arch_remove_reservations(struct resource *avail)
{
}

resource_size_t simple_align_resource(void *data,
					     const struct resource *avail,
					     resource_size_t size,
					     resource_size_t align)
{
	return avail->start;
}

/* this function will try to extend res, 
 * if no enough free space on current level
 * it will try to extend its parent
 */
int extend_res(struct resource *res, resource_size_t size)
{
	struct resource *tmp;
	resource_size_t begin;
	int ret;

again:
	/* first try to extend on its own */
	if (!res->parent)
		return -1; // no parent or root, no place to extend

	tmp = res->parent->child;
	if (tmp != res)
	{
		for (tmp = res->parent->child; tmp; tmp = tmp->sibling)
			if (tmp->sibling == res)
				break;
		begin = tmp->end + 1;
	}
	else
		begin = res->parent->start;

	for ( ; begin < res->start; begin++)
	{
		ret = adjust_resource(res, begin, resource_size(res) + size);
		if (ret == 0)
			return 0;
	}

	/* oops, res couldn't extedn itself in his parent level
	 * try to extend res->parent if res is the first or last
	 * child                             ^_^
	 * */
	if (res->parent->child == res || res->sibling == NULL)
	{
		/* maybe the res->parent doesn't need to extend so much */
		if (extend_res(res->parent, size))
		{
			/* failed to extend parent */
			return -1;
		}
		else
		{
			/* my parent get enough space try again */
			goto again;
		}

	}

	return -1;
}

/*
 * relocate the old resource, to the new position. Return error and do nothing
 * if the new location conflicts with any other resource.
 */
int  __re_request_resource1(struct resource *root,
                       struct resource *new, struct resource *old)
{
       resource_size_t start = new->start;
       resource_size_t end = new->end;
       struct resource *tmp, **p, **p_old  = NULL;

       printf("Debug info: new is %lu-%lu, old is %lu-%lu\n",
            start,end, old->start, old->end);
       if (end < start || start < root->start || end > root->end)
               return -1;
       p = &root->child;
       for (;;) {
               if ( *p == old )
               p_old = p;
               tmp = *p;
               if (!tmp || tmp->start > end) {

                       if (tmp == old) {
                               old->start = start;
                               old->end = end;
                               return 0;
                       }

                       if (!p_old) {
                               p_old = p;
                               while ( *p_old && old !=  (*p_old)->sibling )
                                       p_old =  &(*p_old)->sibling;
                               //BUG_ON(!*p_old);
                   if(!*p_old)
                   {
                       printf("Error!\n");
                   }
                       }

                       /* relocate old */
                       *p_old = old->sibling;
                       old->start = start;
                       old->end   = end;
                       old->sibling = tmp;
                       *p = old;
                       return 0;
               }
               p = &tmp->sibling;
               if (tmp->end >= start)
                       return -1;
        }
 }

int  __re_request_resource2(struct resource *root,
                       struct resource *new, struct resource *old)
{
    resource_size_t start = new->start;
    resource_size_t end = new->end;
    struct resource *iter, **p_iter = NULL;
    struct resource **p_new = NULL;
    struct resource **p_old = NULL;
    
    printf("Debug info: new is %lu-%lu, old is %lu-%lu\n",
            start,end, old->start, old->end);

    if (end < start || start < root->start || end > root->end)
            return -1;
    
    p_iter = &root->child;
    iter = *p_iter;
    
    if ( (start >= old->start && start <= old->end) ||
         (end >= old->start && end <= old->end)) {
            old->start = start;
            old->end   = end;
            return 0;
    }
    
    while ( iter ) {
            if (iter == old) {
                p_old = p_iter;
            }
    
            if (iter->start > end && !p_new) {
                p_new = p_iter;
            }
        else if(iter->sibling == NULL) {
            p_new = &iter->sibling;
        }

        if (p_old && p_new)
            break;

        p_iter = &iter->sibling;
        iter   = *p_iter;
    }
    
    if (p_old == p_new) {
        old->start = start;
        old->end   = end;
        return 0;
    }

    if ((&(*p_old)->sibling == p_new) )  {
        old->start = start;
        old->end   = end;
        return 0;
    }
    
    old->start = start;
    old->end   = end;
    
    *p_old = old->sibling;
    old->sibling = *p_new;
    *p_new = old;

    return 0;
}

void remove_old(struct resource *root, struct resource *old)
{
       struct resource *iter, **p_iter = NULL;
       struct resource **p_old   = NULL;

       p_iter = &root->child;
       iter  = *p_iter;

       while ( iter ) {
           if (iter == old) {
               p_old = p_iter;
               break;
           }

           p_iter = &iter->sibling;
           iter  = *p_iter;
       }

       *p_old = old->sibling;
}

static void resource_clip(struct resource *res, resource_size_t min,
			  resource_size_t max)
{
	if (res->start < min)
		res->start = min;
	if (res->end > max)
		res->end = max;
}

/*
 * Find empty slot in the resource tree with the given range and
 * alignment constraints
 */
/* I think it doesn't search the tree, but just the first level
 * of the *root*.
 * Otherwise it doesn't register in the tree.
 *
 * tmp point to the raw free space find in resource tree
 * avail point to the available space after alignment
 * alloc is the final resource calculated and assigned to new
 *
 * actually it has lot similarity with adjust_resource()
 * */
static int __find_resource(struct resource *root, struct resource *old,
			 struct resource *new,
			 resource_size_t  size,
			 struct resource_constraint *constraint)
{
	struct resource *this = root->child;
	struct resource tmp = *new, avail, alloc;

	tmp.flags = new->flags;
	tmp.start = root->start;
	/*
	 * Skip past an allocated resource that starts at 0, since the assignment
	 * of this->start - 1 to tmp->end below would cause an underflow.
	 */
	/* This condition handle a special case, in which the first child of root
	 * start just from the root->start.
	 * In this case, there is no free space between root and root->child.
	 * If we don't add this special handling, when we jump into the loop,
	 * tmp.start = root->start
	 * tmp.end   = this->start-1 = root->start-1
	 * So you see the underflow.
	 */
	if (this && this->start == root->start) {
		tmp.start = (this == old) ? old->start : this->end + 1;
		this = this->sibling;
	}
	for(;;) {
		if (this)
			tmp.end = (this == old) ?  this->end : this->start - 1;
		else
			tmp.end = root->end;

		resource_clip(&tmp, constraint->min, constraint->max);
		arch_remove_reservations(&tmp);

		/* Check for overflow after ALIGN() */
		avail = *new;
		avail.start = ALIGN(tmp.start, constraint->align);
		avail.end = tmp.end;
		if (avail.start >= tmp.start) {
			alloc.start = constraint->alignf(constraint->alignf_data, &avail,
					size, constraint->align);
			alloc.end = alloc.start + size - 1;
			if (res1_contains_res2(&avail, &alloc)) {
				new->start = alloc.start;
				new->end = alloc.end;
				return 0;
			}
		}
		if (!this)
			break;
		if (this != old)
			tmp.start = this->end + 1;
		this = this->sibling;
	}
	return -1;
}

/*
 * Find empty slot in the resource tree given range and alignment.
 */
int find_resource(struct resource *root, struct resource *new,
			resource_size_t size,
			struct resource_constraint  *constraint)
{
	return  __find_resource(root, NULL, new, size, constraint);
}

/* Return the conflict entry if you can't request it */
/* this function just search the child level, not the grand-child level */
static struct resource * __request_resource(struct resource *root, struct resource *new)
{
	resource_size_t start = new->start;
	resource_size_t end = new->end;
	struct resource *tmp, **p;

	/* check whether the request is reasonable */
	if (end < start)
		return root;
	/* check whether the request is out of the root */
	if (start < root->start)
		return root;
	if (end > root->end)
		return root;

	/* go though the children */
	p = &root->child;
	for (;;) {
		tmp = *p;
		if (!tmp || tmp->start > end) {
			/* insert new before tmp */
			new->sibling = tmp;
			*p = new;
			new->parent = root;
			return NULL;
		}
		p = &tmp->sibling;
		if (tmp->end < start)
			continue;
		return tmp;
	}
}

struct resource *request_resource_conflict(struct resource *root, struct resource *new)
{
	struct resource *conflict;

	//write_lock(&resource_lock);
	conflict = __request_resource(root, new);
	//write_unlock(&resource_lock);
	return conflict;
}

/*
 * Insert a resource into the resource tree. If successful, return NULL,
 * otherwise return the conflicting resource (compare to __request_resource())
 */
static struct resource * __insert_resource(struct resource *parent, struct resource *new)
{
	struct resource *first, *next;

	for (;; parent = first) {
		first = __request_resource(parent, new);
		if (!first) /* well no one occupy the space */
			return first; /* here first is null */

		if (first == parent)
			return first;
		if (first == new)	/* duplicated insertion */
			return first;

		/* first doesn't contains new */
		if ((first->start > new->start) || (first->end < new->end))
			break;
		/* first equals new */
		if ((first->start == new->start) && (first->end == new->end))
			break;

		/* code come to here means the new is exactly contained in first
		 * then dig into to the "first"
		 * */
	}

	/* code come here with just two cases
	 * 1. new partially overlap with first
	 * 2. new contains first or contains first and some of his siblings
	 * */
	for (next = first; ; next = next->sibling) {
		/* Partial overlap? Bad, and unfixable */
		if (next->start < new->start || next->end > new->end)
			return next;
		if (!next->sibling)
			break;
		if (next->sibling->start > new->end)
			break;
	}

	/* now fisrt-next are covered by new */

	new->parent = parent;
	new->sibling = next->sibling;
	new->child = first;

	next->sibling = NULL;
	for (next = first; next; next = next->sibling)
		next->parent = new;

	if (parent->child == first) {
		parent->child = new;
	} else {
		next = parent->child;
		while (next->sibling != first)
			next = next->sibling;
		next->sibling = new;
	}
	return NULL;
}

struct resource *insert_resource_conflict(struct resource *parent, struct resource *new)
{
	struct resource *conflict;

	conflict = __insert_resource(parent, new);

	return conflict;
}

static void __release_child_resources(struct resource *r)
{
	struct resource *tmp, *p;
	resource_size_t size;

	p = r->child;
	r->child = NULL;
	while (p) {
		tmp = p;
		p = p->sibling;

		tmp->parent = NULL;
		tmp->sibling = NULL;
		__release_child_resources(tmp);

		//printf("release child resource %pR\n", tmp);
		/* need to restore size, and keep flags */
		size = resource_size(tmp);
		tmp->start = 0;
		tmp->end = size - 1;
	}
}

void release_child_resources(struct resource *r)
{
	__release_child_resources(r);
}

static int __release_resource(struct resource *old)
{
	struct resource *tmp, **p;

	p = &old->parent->child;
	for (;;) {
		tmp = *p;
		if (!tmp)
			break;
		if (tmp == old) {
			*p = tmp->sibling;
			old->parent = NULL;
			return 0;
		}
		p = &tmp->sibling;
	}
	return -1;
}

/* This function just release the old itself
 * need to call release_child_resources to release
 * the children*/
int release_resource(struct resource *old)
{
	int retval;

	//write_lock(&resource_lock);
	retval = __release_resource(old);
	//write_unlock(&resource_lock);
	return retval;
}

/**
 * reallocate_resource - allocate a slot in the resource tree given range & alignment.
 *	The resource will be relocated if the new size cannot be reallocated in the
 *	current location.
 *
 * @root: root resource descriptor
 * @old:  resource descriptor desired by caller
 * @newsize: new size of the resource descriptor
 * @constraint: the size and alignment constraints to be met.
 */
int reallocate_resource(struct resource *root, struct resource *old,
			resource_size_t newsize,
			struct resource_constraint  *constraint)
{
	int err=0;
	struct resource new = *old;
	struct resource *conflict;

	//write_lock(&resource_lock);

	if ((err = __find_resource(root, old, &new, newsize, constraint)))
		goto out;

	if (res1_contains_res2(&new, old)) {
		old->start = new.start;
		old->end = new.end;
		goto out;
	}

	if (old->child) {
		err = -1;
		goto out;
	}

	if (res1_contains_res2(old, &new)) {
		old->start = new.start;
		old->end = new.end;
	} else {
		__release_resource(old);
		*old = new;
		conflict = __request_resource(root, old);
		//BUG_ON(conflict);
	}
out:
	printf("reallocate_resource: __find_resource get new res of %lu-%lu\n ",
		new.start, new.end);
	//write_unlock(&resource_lock);
	return err;
}

/**
 * allocate_resource - allocate empty slot in the resource tree given range & alignment.
 * 	The resource will be reallocated with a new size if it was already allocated
 * @root: root resource descriptor
 * @new: resource descriptor desired by caller
 * @size: requested resource region size
 * @min: minimum size to allocate
 * @max: maximum size to allocate
 * @align: alignment requested, in bytes
 * @alignf: alignment function, optional, called if not NULL
 * @alignf_data: arbitrary data to pass to the @alignf function
 */
int allocate_resource(struct resource *root, struct resource *new,
		      resource_size_t size, resource_size_t min,
		      resource_size_t max, resource_size_t align,
		      resource_size_t (*alignf)(void *,
						const struct resource *,
						resource_size_t,
						resource_size_t),
		      void *alignf_data)
{
	int err;
	struct resource_constraint constraint;

	if (!alignf)
		alignf = simple_align_resource;

	constraint.min = min;
	constraint.max = max;
	constraint.align = align;
	constraint.alignf = alignf;
	constraint.alignf_data = alignf_data;

	if ( new->parent ) {
		/* resource is already allocated, try reallocating with
		   the new constraints */
		return reallocate_resource(root, new, size, &constraint);
	}

	//write_lock(&resource_lock);
	err = find_resource(root, new, size, &constraint);
	if (err >= 0 && __request_resource(root, new))
		err = -1;
	//write_unlock(&resource_lock);
	return err;
}

/**
 * adjust_resource - modify a resource's start and size
 * @res: resource to modify
 * @start: new start value
 * @size: new size
 *
 * Given an existing resource, change its start and size to match the
 * arguments.  Returns 0 on success, -EBUSY if it can't fit.
 * Existing children of the resource are assumed to be immutable.
 */
int adjust_resource(struct resource *res, resource_size_t start, resource_size_t size)
{
	struct resource *tmp, *parent = res->parent;
	resource_size_t end = start + size - 1;
	int result = -1;

	//write_lock(&resource_lock);

	/* check it doesn't exceed the parent range */
	if ((start < parent->start) || (end > parent->end))
		goto out;

	/* check it cover children */
	for (tmp = res->child; tmp; tmp = tmp->sibling) {
		if ((tmp->start < start) || (tmp->end > end))
			goto out;
	}

	/* check it will not beat younger brother */
	if (res->sibling && (res->sibling->start <= end))
		goto out;

	/* check it will not beat older brother */
	tmp = parent->child;
	if (tmp != res) {
		while (tmp->sibling != res)
			tmp = tmp->sibling;
		if (start <= tmp->end)
			goto out;
	}

	/* ok, every thing is fine, adjust it */
	res->start = start;
	res->end = end;
	result = 0;

 out:
	//write_unlock(&resource_lock);
	return result;
}

#if 0
int probe_resource(struct resource *b_res,
			 /*  struct device *dev,*/ struct resource *busn_res,
			 resource_size_t needed_size, struct resource **p,
			 int skip_nr, int limit, char *name)
{
	int ret = -1;
	resource_size_t n_size;
	struct resource *parent_res = NULL;
	resource_size_t tmp = b_res->end + 1;

again:
	/*
	 * find biggest range in b_res that we can use in the middle
	 *  and we can not use some spots from start of b_res.
	 */
	n_size = resource_size(b_res);
	if (n_size > skip_nr)
		n_size -= skip_nr;
	else
		n_size = 0;

	memset(busn_res, 0, sizeof(struct resource));
	//dev_printk(KERN_DEBUG, dev, "find free %s in res: %pR\n", name, b_res);
	while (n_size >= needed_size) {
		ret = allocate_resource(b_res, busn_res, n_size,
				b_res->start + skip_nr, b_res->end,
				1, NULL, NULL);
		if (!ret)
			return ret;
		n_size--;
	}

	/* try extend the top of parent bus, find free under top at first */
	//write_lock(&resource_lock);
	n_size = __find_res_top_free_size(b_res);
	//dev_printk(KERN_DEBUG, dev, "found free %s %ld in res: %pR top\n",
	//		name, (unsigned long)n_size, b_res);

	/* can not extend cross local boundary */
	if ((limit - b_res->end) < (needed_size - n_size))
		goto reduce_needed_size;

	/* find extended range */
	memset(busn_res, 0, sizeof(struct resource));
	parent_res = b_res;
	while (parent_res) {
		ret = __allocate_resource(parent_res, busn_res,
			 needed_size - n_size,
			 tmp, tmp + needed_size - n_size - 1,
			 1, NULL, NULL, false);
		if (!ret) {
			/* save parent_res, we need it as stopper later */
			*p = parent_res;

			/* prepare busn_res for return */
			__release_resource(busn_res);
			busn_res->start -= n_size;

			/* extend parent bus top*/
			__resource_extend_parents_top(b_res,
					 needed_size - n_size, parent_res);
			__request_resource(b_res, busn_res);

			//write_unlock(&resource_lock);
			return ret;
		}
		parent_res = parent_res->parent;
	}

reduce_needed_size:
	//write_unlock(&resource_lock);
	/* ret must not be 0 here */
	needed_size--;
	if (needed_size)
		goto again;

	return ret;
}
#endif

resource_size_t __find_res_top_free_size(struct resource *res)
{
	resource_size_t n_size;
	struct resource tmp_res;

	/*
	 *   find out number below res->end, that we can use at first
	 *	res->start can not be used.
	 */
	n_size = resource_size(res) - 1;
	memset(&tmp_res, 0, sizeof(struct resource));
	while (n_size > 0) {
		int ret;

		ret = allocate_resource(res, &tmp_res, n_size,
			res->end - n_size + 1, res->end,
			1, NULL, NULL/*  , false*/);
		if (ret == 0) {
			__release_resource(&tmp_res);
			break;
		}
		n_size--;
	}

	return n_size;
}

void __resource_update_parents_top(struct resource *b_res,
		 long size, struct resource *parent_res)
{
	struct resource *res = b_res;

	if (!size)
		return;

	while (res) {
		if (res == parent_res)
			break;
		res->end += size;
		res = res->parent;
	}
}

void __resource_extend_parents_top(struct resource *b_res,
		 long size, struct resource *parent_res)
{
	__resource_update_parents_top(b_res, size, parent_res);
}

/* this function will go deep in the resource tree to allocate a resource 
 * compare __request_region() with __request_resource().
 * __request_resource will just go through the first level, while 
 * __request_region will go deep into the resource tree and find a proper one.
 * */
struct resource * __request_region(struct resource *parent,
				   resource_size_t start, resource_size_t n,
				   const char *name, int flags)
{
	//DECLARE_WAITQUEUE(wait, current);
	struct resource *res = malloc(sizeof(*res));
	memset(res, sizeof(*res), 0);

	if (!res)
		return NULL;

	res->name = name;
	res->start = start;
	res->end = start + n - 1;
	res->flags = IORESOURCE_BUSY;
	res->flags |= flags;

	//write_lock(&resource_lock);

	for (;;) {
		struct resource *conflict;

		conflict = __request_resource(parent, res);
		if (!conflict)
			break;
		if (conflict != parent) {
			parent = conflict;
			if (!(conflict->flags & IORESOURCE_BUSY))
				continue;
		}
		if (conflict->flags & flags & IORESOURCE_MUXED) {
			//add_wait_queue(&muxed_resource_wait, &wait);
			//write_unlock(&resource_lock);
			//set_current_state(TASK_UNINTERRUPTIBLE);
			//schedule();
			//remove_wait_queue(&muxed_resource_wait, &wait);
			//write_lock(&resource_lock);
			continue;
		}
		/* Uhhuh, that didn't work out.. */
		free(res);
		res = NULL;
		break;
	}
	//write_unlock(&resource_lock);
	return res;
}

static void __reserve_region_with_split1(struct resource *root,
		resource_size_t start, resource_size_t end,
		const char *name)
{
	struct resource *parent = root;
	struct resource *conflict;
	struct resource *res = malloc(sizeof(*res));
	struct resource *next_res = NULL;

	if (!res)
		return;

	memset(res, 0, sizeof(*res));
	res->name = name;
	res->start = start;
	res->end = end;
	res->flags = IORESOURCE_BUSY;

	while (1) {

		conflict = __request_resource(parent, res);
		if (!conflict) {
			if (!next_res)
				break;
			res = next_res;
			next_res = NULL;
			continue;
		}

		/* conflict covered whole area */
		if (conflict->start <= res->start &&
				conflict->end >= res->end) {
			free(res);
			//WARN_ON(next_res);
			break;
		}

		/* failed, split and try again */
		if (conflict->start > res->start) {
			end = res->end;
			res->end = conflict->start - 1;
			if (conflict->end < end) {
				next_res = malloc(sizeof(*res));
				if (!next_res) {
					free(res);
					break;
				}
				memset(res, 0, sizeof(*next_res));
				next_res->name = name;
				next_res->start = conflict->end + 1;
				next_res->end = end;
				next_res->flags = IORESOURCE_BUSY;
			}
		} else {
			res->start = conflict->end + 1;
		}
	}

}

void reserve_region_with_split1(struct resource *root,
		resource_size_t start, resource_size_t end,
		const char *name)
{
	int abort = 0;

	//write_lock(&resource_lock);
	if (root->start > start || root->end < end) {
		printf("requested range [0x%llx-0x%llx] not in root %pr\n",
		       (unsigned long long)start, (unsigned long long)end,
		       root);
		if (start > root->end || end < root->start)
			abort = 1;
		else {
			if (end > root->end)
				end = root->end;
			if (start < root->start)
				start = root->start;
			printf("fixing request to [0x%llx-0x%llx]\n",
			       (unsigned long long)start,
			       (unsigned long long)end);
		}
		//dump_stack();
	}
	if (!abort)
		__reserve_region_with_split1(root, start, end, name);
	//write_unlock(&resource_lock);
}

static void __reserve_region_with_split(struct resource *root,
		resource_size_t start, resource_size_t end,
		const char *name)
{
	struct resource *parent = root;
	struct resource *conflict;
	struct resource *res = malloc(sizeof(*res));

	if (!res)
		return;

	memset(res, 0, sizeof(*res));
	res->name = name;
	res->start = start;
	res->end = end;
	res->flags = IORESOURCE_BUSY;

	conflict = __request_resource(parent, res);
	if (!conflict)
		return;

	/* failed, split and try again */
	free(res);

	/* conflict covered whole area */
	if (conflict->start <= start && conflict->end >= end)
		return;

	if (conflict->start > start)
		__reserve_region_with_split(root, start, conflict->start-1, name);
	if (conflict->end < end)
		__reserve_region_with_split(root, conflict->end+1, end, name);
}

void reserve_region_with_split(struct resource *root,
		resource_size_t start, resource_size_t end,
		const char *name)
{
	int abort = 0;

	//write_lock(&resource_lock);
	if (root->start > start || root->end < end) {
		printf("requested range [0x%llx-0x%llx] not in root %pr\n",
		       (unsigned long long)start, (unsigned long long)end,
		       root);
		if (start > root->end || end < root->start)
			abort = 1;
		else {
			if (end > root->end)
				end = root->end;
			if (start < root->start)
				start = root->start;
			printf("fixing request to [0x%llx-0x%llx]\n",
			       (unsigned long long)start,
			       (unsigned long long)end);
		}
		//dump_stack();
	}
	if (!abort)
		__reserve_region_with_split(root, start, end, name);
	//write_unlock(&resource_lock);
}

struct resource *next_resource(struct resource *p, bool sibling_only)
{
	/* Caller wants to traverse through siblings only */
	if (sibling_only)
		return p->sibling;

	if (p->child)
		return p->child;
	while (!p->sibling && p->parent)
		p = p->parent;
	return p->sibling;
}

static int find_next_iomem_res(struct resource *res, //unsigned long desc,
			       bool first_level_children_only)
{
	struct resource *p;

	//BUG_ON(!res);
	//BUG_ON(res->start >= res->end);

	//read_lock(&resource_lock);
	//printf("%s:%lu-%lu  \n", __func__, res->start, res->end);

	for (p = iomem_resource.child; p;
		p = next_resource(p, first_level_children_only)) {
		//if ((p->flags & res->flags) != res->flags)
		//	continue;
		//if ((desc != IORES_DESC_NONE) && (desc != p->desc))
		//	continue;
		if (p->start > res->end) {
			p = NULL;
			break;
		}
		if ((p->end >= res->start) && (p->start < res->end))
			break;
	}

	//read_unlock(&resource_lock);
	if (!p)
		return -1;
	/* copy data */
	resource_clip(res, p->start, p->end);
	res->flags = p->flags;
	//res->desc = p->desc;
	return 0;
}

static int __walk_iomem_res_desc(struct resource *res, //unsigned long desc,
				 bool first_level_children_only,
				 void *arg,
				 int (*func)(struct resource *, void *))
{
	resource_size_t orig_end = res->end;
	int ret = -1;

	while ((res->start < res->end) &&
	       !find_next_iomem_res(res, /* desc, */ first_level_children_only)) {
		ret = (*func)(res, arg);
		if (ret)
			break;

		res->start = res->end + 1;
		res->end = orig_end;
	}

	return ret;
}

int walk_iomem_res_desc(unsigned long desc, unsigned long flags, resource_size_t start,
		resource_size_t end, void *arg, int (*func)(struct resource *, void *))
{
	struct resource res;

	res.start = start;
	res.end = end;
	res.flags = flags;

	return __walk_iomem_res_desc(&res, /* desc, */ false, arg, func);
}

static int __walk_iomem_res_desc_rev2(struct resource *self, struct resource *res, //unsigned long desc,
				 bool first_level_children_only, void *arg,
				 int (*func)(struct resource *, void *))
{
	resource_size_t orig_start = res->start;
	int ret = -1;

	if (!self || (res->start >= res->end)) {
		return 0;
	}

	ret = __walk_iomem_res_desc_rev2(self->sibling, res,
			first_level_children_only, arg, func);

	if ((self->end >= res->start) && (self->start < res->end)) {
		ret = (*func)(self, arg);
		res->end = self->start;
		res->start = orig_start;
	}

	if (first_level_children_only) {
		return ret;
	}

	ret = __walk_iomem_res_desc_rev2(self->child, res,
			first_level_children_only, arg, func);

	return ret;
}

struct resource *prev_resource(struct resource *p, bool sibling_only)
{
	struct resource *prev;
	if (NULL == iomem_resource.child)
		return NULL;

	if (p == NULL) {
		prev = iomem_resource.child;
		while (prev->sibling)
			prev = prev->sibling;
	} else {
		/* p is the first child, return parent */
		if (p->parent->child == p) {
			return p->parent;
		}

		/* p is not the first child, get elder brother */
		for (prev = p->parent->child; prev->sibling != p;
			prev = prev->sibling) {}
	}

	/* Caller wants to traverse through siblings only */
	if (sibling_only)
		return prev;

	for (;prev->child;) {
		prev = prev->child;

		while (prev->sibling)
			prev = prev->sibling;
	}
	return prev;
}

static int find_prev_iomem_res(struct resource *res, //unsigned long desc,
			       bool first_level_children_only)
{
	struct resource *p;

	//BUG_ON(!res);
	//BUG_ON(res->start >= res->end);

	//read_lock(&resource_lock);
	//printf("%s:%lu-%lu  \n", __func__, res->start, res->end);

	for (p = prev_resource(NULL, first_level_children_only); p;
		p = prev_resource(p, first_level_children_only)) {
		//if ((p->flags & res->flags) != res->flags)
		//	continue;
		//if ((desc != IORES_DESC_NONE) && (desc != p->desc))
		//	continue;
		if (p->end < res->start) {
			p = NULL;
			break;
		}
		if ((p->end >= res->start) && (p->start < res->end))
			break;
	}

	//read_unlock(&resource_lock);
	if (!p)
		return -1;
	/* copy data */
	resource_clip(res, p->start, p->end);
	res->flags = p->flags;
	//res->desc = p->desc;
	return 0;
}

static int __walk_iomem_res_desc_rev(struct resource *res, //unsigned long desc,
				 bool first_level_children_only, void *arg,
				 int (*func)(struct resource *, void *))
{
	resource_size_t orig_start = res->start;
	int ret = -1;

	while ((res->start < res->end) &&
	       !find_prev_iomem_res(res, /* desc, */ first_level_children_only)) {
		ret = (*func)(res, arg);
		if (ret)
			break;

		res->end = res->start - 1;
		res->start = orig_start;
	}

	return ret;
}

int walk_iomem_res_desc_rev(unsigned long desc, unsigned long flags, resource_size_t start,
		resource_size_t end, void *arg, int (*func)(struct resource *, void *))
{
	struct resource res;

	res.start = start;
	res.end = end;
	res.flags = flags;

	//return __walk_iomem_res_desc_rev2(iomem_resource.child, &res, /* desc, */ false, arg, func);
	return __walk_iomem_res_desc_rev(&res, /* desc, */ false, arg, func);
}

