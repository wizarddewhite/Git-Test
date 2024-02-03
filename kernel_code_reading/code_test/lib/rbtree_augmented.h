/*
 * =====================================================================================
 *
 *       Filename:  rbtree_augmented.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/22/2015 14:48:02
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef _RBTREE_AUGMENTED_H
#define _RBTREE_AUGMENTED_H

#include "rb_tree.h"

struct rb_augment_callbacks {
	void (*propagate)(struct rb_node *node, struct rb_node *stop);
	void (*copy)(struct rb_node *old, struct rb_node *new);
	void (*rotate)(struct rb_node *old, struct rb_node *new);
};

#define __rb_parent(pc)    ((struct rb_node *)(pc & ~3))

#define __rb_color(pc)     ((pc) & 1)
#define __rb_is_black(pc)  __rb_color(pc)
#define __rb_is_red(pc)    (!__rb_color(pc))
#define rb_color(rb)       __rb_color((rb)->__rb_parent_color)
#define rb_is_red(rb)      __rb_is_red((rb)->__rb_parent_color)
#define rb_is_black(rb)    __rb_is_black((rb)->__rb_parent_color)

static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
	rb->__rb_parent_color = rb_color(rb) | (unsigned long)p;
}

static struct rb_node *
__rb_erase_augmented(struct rb_node *node, struct rb_root *root,
		     const struct rb_augment_callbacks *augment)
{
	struct rb_node *child = node->rb_right;
	struct rb_node *tmp = node->rb_left;
	struct rb_node *parent, *rebalance;

	if (!tmp) {
		/*
		 * Case 1: node to erase has no more than 1 child (easy!)
		 *
		 * Note that if there is one child it must be red due to 5)
		 * and node must be black due to 4). We adjust colors locally
		 * so as to bypass __rb_erase_color() later on.
		 */
		parent = rb_parent(node);
		__rb_change_child(node, child, parent, root);
		if (child) {
			rb_set_parent_color(child, parent, rb_color(node));
			rebalance = NULL;
		} else
			rebalance = rb_is_black(node) ? parent : NULL;
		tmp = parent;
	} else if (!child) {
		/* Still case 1, but this time the child is node->rb_left */
		parent = rb_parent(node);
		__rb_change_child(node, tmp, parent, root);
		/*
		 * if (!tmp) {
		 */
			rb_set_parent_color(tmp, parent, rb_color(node));
			rebalance = NULL;
		/*
		 * } else
		 * 	rebalance = rb_is_black(node) ? parent : NULL;
		 */
		tmp = parent;
	} else {
		struct rb_node *successor = child, *child2;

		tmp = child->rb_left;
		if (!tmp) {
			/*
			 * Case 2: node's successor is its right child
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (s)  ->  (x) (c)
			 *        \
			 *        (c)
			 */
			parent = successor;
			child2 = successor->rb_right;

			augment->copy(node, successor);
		} else {
			/*
			 * Case 3: node's successor is leftmost under
			 * node's right child subtree
			 *
			 *    (n)          (s)
			 *    / \          / \
			 *  (x) (y)  ->  (x) (y)
			 *      /            /
			 *    (p)          (p)
			 *    /            /
			 *  (s)          (c)
			 *    \
			 *    (c)
			 */
			do {
				parent = successor;
				successor = tmp;
				tmp = tmp->rb_left;
			} while (tmp);
			child2 = successor->rb_right;
			/* remove successor */
			parent->rb_left = child2;
			/* then replace it with node */
			successor->rb_right = child;
			rb_set_parent(child, successor);

			augment->copy(node, successor);
			augment->propagate(parent, successor);
		}

		tmp = node->rb_left;
		successor->rb_left = tmp;
		rb_set_parent(tmp, successor);

		tmp = rb_parent(node);
		__rb_change_child(node, successor, tmp, root);

		if (child2) {
			rb_set_parent_color(child2, parent, RB_BLACK);
			rebalance = NULL;
		} else
			rebalance = rb_is_black(successor) ? parent : NULL;
		rb_set_parent_color(successor, tmp, rb_color(node));
		tmp = successor;
	}

	augment->propagate(tmp, NULL);
	return rebalance;
}

static void
rb_erase_augmented(struct rb_node *node, struct rb_root *root,
		   const struct rb_augment_callbacks *augment)
{
	struct rb_node *rebalance = __rb_erase_augmented(node, root, augment);
	if (rebalance)
		__rb_erase_color(rebalance, root, augment->rotate);
}

#define RB_DECLARE_CALLBACKS(rbstatic, rbname, rbstruct, rbfield,	\
			     rbtype, rbaugmented, rbcompute)		\
static inline void							\
rbname ## _propagate(struct rb_node *rb, struct rb_node *stop)		\
{									\
	while (rb != stop) {						\
		rbstruct *node = rb_entry(rb, rbstruct, rbfield);	\
		rbtype augmented = rbcompute(node);			\
		if (node->rbaugmented == augmented)			\
			break;						\
		node->rbaugmented = augmented;				\
		rb = rb_parent(&node->rbfield);				\
	}								\
}									\
static inline void							\
rbname ## _copy(struct rb_node *rb_old, struct rb_node *rb_new)		\
{									\
	rbstruct *old = rb_entry(rb_old, rbstruct, rbfield);		\
	rbstruct *new = rb_entry(rb_new, rbstruct, rbfield);		\
	new->rbaugmented = old->rbaugmented;				\
}									\
static void								\
rbname ## _rotate(struct rb_node *rb_old, struct rb_node *rb_new)	\
{									\
	rbstruct *old = rb_entry(rb_old, rbstruct, rbfield);		\
	rbstruct *new = rb_entry(rb_new, rbstruct, rbfield);		\
	new->rbaugmented = old->rbaugmented;				\
	old->rbaugmented = rbcompute(old);				\
}									\
rbstatic const struct rb_augment_callbacks rbname = {			\
	.propagate = rbname ## _propagate,				\
	.copy = rbname ## _copy,					\
	.rotate = rbname ## _rotate					\
};

/*
 * Fixup the rbtree and update the augmented information when rebalancing.
 *
 * On insertion, the user must update the augmented information on the path
 * leading to the inserted node, then call rb_link_node() as usual and
 * rb_insert_augmented() instead of the usual rb_insert_color() call.
 * If rb_insert_augmented() rebalances the rbtree, it will callback into
 * a user provided function to update the augmented information on the
 * affected subtrees.
 */
static inline void
rb_insert_augmented(struct rb_node *node, struct rb_root *root,
		    const struct rb_augment_callbacks *augment)
{
	__rb_insert_augmented(node, root, augment->rotate);
}
#endif // _RBTREE_AUGMENTED_H 
