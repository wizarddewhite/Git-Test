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
#define __re_request_resource __re_request_resource2


struct resource root;
struct resource res[10];

void init()
{
    int i;

    root.start = 0;
    root.end   = 1000;

    res[0].start = 10;
    res[0].end   = 50;
    
    res[1].start = 90;
    res[1].end   = 150;
    
    res[2].start = 210;
    res[2].end   = 250;

    res[3].start = 310;
    res[3].end   = 450;

    /*link it*/
    root.child = &res[0];

    for(i = 0; i< 3; i++)
    {
        res[i].sibling = &res[i+1];
    }

    res[3].sibling = NULL;

}

void dump(struct resource *root)
{
    struct resource **p     = &root->child;
    struct resource *tmp    = NULL;

    while (*p)
    {
        tmp = *p;
        printf("%lu-%lu\n", tmp->start, tmp->end);
        p = &tmp->sibling;
    }
}

/*
 * relocate the old resource, to the new position. Return error and do nothing
 * if the new location conflicts with any other resource.
 */
static int  __re_request_resource1(struct resource *root,
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

static int  __re_request_resource2(struct resource *root,
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



static int  remove_old(struct resource *root, struct resource *old)
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

	for (next = first; ; next = next->sibling) {
		/* Partial overlap? Bad, and unfixable */
		if (next->start < new->start || next->end > new->end)
			return next;
		if (!next->sibling)
			break;
		if (next->sibling->start > new->end)
			break;
	}

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

void realloc_test()
{
    int index;

    /*test cases for resoures*/
    dump(&root);
    struct resource new;
    scanf("%d", &index);
    /* 10-50, 90-150, 210-250, 310-450 */
    switch (index)
    {
        case 1: // expand the range, left side
            new.start = 5;
            new.end   = 50;
            __re_request_resource(&root, &new, &res[0]);
            break;
        case 2: // expand the range, right side
            new.start = 10;
            new.end   = 60;
            __re_request_resource(&root, &new, &res[0]);
            break;
        case 3: // expand the range, both side
            new.start = 7;
            new.end   = 80;
            __re_request_resource(&root, &new, &res[0]);
            break;
        case 4: // allocate a new range, ahead of old
            new.start = 60;
            new.end   = 80;
            __re_request_resource(&root, &new, &res[1]);
            break;
        case 5: // allocate a new range, after old
            new.start = 160;
            new.end   = 200;
            __re_request_resource(&root, &new, &res[1]);
            break;
        case 6: // allocate a new range, at the beginning
            new.start = 2;
            new.end   = 8;
            __re_request_resource(&root, &new, &res[2]);
            break;
        case 7: // allocate a new range, at last
            new.start = 500;
            new.end   = 600;
            __re_request_resource(&root, &new, &res[2]);
            break;
        case 8: // allocate a new range, at last with old is the last too
            new.start = 500;
            new.end   = 600;
            __re_request_resource(&root, &new, &res[3]);
            break;
        default:
            for (index = 0; index < 3; index++)
                printf("%x\n", (unsigned int)&res[index]);
            printf("------------\n");
            remove_old(&root, &res[2]);
            break;
    }

    dump(&root);

    return;
}

void relation_test()
{
	struct resource res1, res2;

	res1.start = 5;
	res1.end = 10;

	res2.start = 5;
	res2.end = 10;

	printf("res the same %d\n", res1_contains_res2(&res1, &res2));

	res2.start = 6;
	res2.end = 10;
	printf("res1 contain res2 %d\n", res1_contains_res2(&res1, &res2));

	res2.start = 5;
	res2.end = 11;
	printf("res1 contain res2 %d\n", res1_contains_res2(&res1, &res2));

	printf("res contain %d\n", resource_contains(&res1, &res2));

}

int main()
{

    init();

    relation_test();


    return 0;
}
