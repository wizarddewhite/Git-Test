/*
 * =====================================================================================
 *
 *       Filename:  rb_tree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2012 11:38:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/rbtree_augmented.h>

#include "rb_tree.h"

MODULE_LICENSE("Dual BSD/GPL");

#define NODES       6

static struct rb_root tree_root = RB_ROOT;

struct dummy_struct tree_nodes[NODES];

void dump_rbtree_augmented(struct rb_node *node, int level, enum child_dir state)
{
	struct dummy_struct *this;
	char prefix[40] = {0};
	int  i;

	if (!node)
		return;

	dump_rbtree_augmented(node->rb_right, level+1, right_child);

	for (i = 0; i < level && i < 10; i++)
		strcat(prefix, "   ");

	if (state == left_child)
		printk(KERN_ERR "   %s| \n", prefix);
	this = rb_to_dummy(node);
	printk(KERN_ERR "%02d %s -%02d(%c):%d\n",
			level, prefix, this->idx,
			rb_is_red(node)?'r':'b', this->subtree_nodes);
	if (state == right_child)
		printk(KERN_ERR "   %s| \n", prefix);

	dump_rbtree_augmented(node->rb_left, level+1, left_child);
}

static inline u32 subtree_node_recompute(struct dummy_struct *node)
{
	u32 child_subtree_nodes = 1;
	if (node->rb.rb_left)
		child_subtree_nodes += rb_entry(node->rb.rb_left, struct dummy_struct,
					   rb)->subtree_nodes;
	if (node->rb.rb_right)
		child_subtree_nodes += rb_entry(node->rb.rb_right, struct dummy_struct,
					   rb)->subtree_nodes;
	return child_subtree_nodes;
}

RB_DECLARE_CALLBACKS(static, augment_callbacks, struct dummy_struct, rb,
		     u32, subtree_nodes, subtree_node_recompute)

static int insert_augmented_to_tree(struct dummy_struct *node)
{
	struct rb_node **new, *parent;

	new = &tree_root.rb_node;
	parent = NULL;

	/* Init the new added node's subtree_nodes to 1 */
	node->subtree_nodes = 1;

	while (*new) {
		struct dummy_struct *this;
		this = rb_to_dummy(*new);

		parent = *new;
		/* Parent's subtree_nodes increase 1 */
		rb_to_dummy(parent)->subtree_nodes++;

		if (node->idx > this->idx)
			new = &((*new)->rb_right);
		else
			new = &((*new)->rb_left);
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_augmented(&node->rb, &tree_root, &augment_callbacks);

	return 0;
}

static void init(void)
{
	int i;
	tree_root = RB_ROOT;
	for(i = 0; i < NODES; i++) {
		tree_nodes[i].idx = i;
		tree_nodes[i].rb.rb_right = NULL;
		tree_nodes[i].rb.rb_left = NULL;
		tree_nodes[i].rb.__rb_parent_color = 0;
		tree_nodes[i].subtree_nodes = 0;
	}
}

static int rb_tree_init(void)
{
	int i;

	printk(KERN_ALERT "Hello, world \n");

	/* init the tree nodes */
	init();

	for(i = 0; i < NODES; i++)
		insert_augmented_to_tree(&tree_nodes[i]);

	dump_rbtree_augmented(tree_root.rb_node, 0, root_node);

	return 0;
}
static void rb_tree_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(rb_tree_init);
module_exit(rb_tree_exit);
