/*
 * =====================================================================================
 *
 *       Filename:  rb_tree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/29/2015 03:55:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef	_RBTREE_H
#define	_RBTREE_H

#include <stddef.h>
#include <stdbool.h>

#define container_of(ptr, type, member) ({                      \
	const typeof( ((type *)0)->member) *__mptr = (ptr);     \
	(type *)( (char*)__mptr -  offsetof(type, member) );})

#define	RB_RED		0
#define	RB_BLACK	1

#define rb_parent(r)   ((struct rb_node *)((r)->__rb_parent_color & ~3))
#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#define __rb_color(pc)     ((pc) & 1)
#define __rb_is_black(pc)  __rb_color(pc)
#define __rb_is_red(pc)    (!__rb_color(pc))
#define rb_color(rb)       __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb)      __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb)    __rb_is_black((rb)->__rb_parent_color)

#define RB_EMPTY_NODE(node)  \
	((node)->__rb_parent_color == (unsigned long)(node))

struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node *rb_right;
	struct rb_node *rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
	struct rb_node *rb_node;
};

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
				struct rb_node **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

static inline void rb_set_parent_color(struct rb_node *rb,
				       struct rb_node *p, int color)
{
	rb->__rb_parent_color = (unsigned long)p | color;
}

static inline void
__rb_change_child(struct rb_node *old, struct rb_node *new,
		  struct rb_node *parent, struct rb_root *root)
{
	if (parent) {
		if (parent->rb_left == old)
			parent->rb_left = new;
		else
			parent->rb_right = new;
	} else
		root->rb_node = new;
}
struct rb_node *rb_first(const struct rb_root *root);
struct rb_node *rb_next(const struct rb_node *node);
struct rb_node *rb_prev(const struct rb_node *node);
#endif	//_RBTREE_H
