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

#include "resource.h"
#include "resource_case.h"

void dump(struct resource *root, int level)
{
	struct resource *tmp    = NULL;

	if (!root)
		return;

	/* print itself first */
	printf("%*c%lu-%lu\n",level*3, ' ', root->start, root->end);

	/* depth first */
	dump(root->child, level+1);

	/* then brothers */
	dump(root->sibling, level);

	return;
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
    
    if ( (p_old == p_new) ) {
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



int  remove_old(struct resource *root, struct resource *old)
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

/* Return the conflict entry if you can't request it */
/* this function just search the child level, not the grand-child level */
static struct resource * __request_resource(struct resource *root, struct resource *new)
{
	resource_size_t start = new->start;
	resource_size_t end = new->end;
	struct resource *tmp, **p;

	if (end < start)
		return root;
	if (start < root->start)
		return root;
	if (end > root->end)
		return root;
	p = &root->child;
	for (;;) {
		tmp = *p;
		if (!tmp || tmp->start > end) {
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
		if ((first == new))	/* duplicated insertion */
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
