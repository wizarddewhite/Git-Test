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

#define NODES       10

static struct rb_root tree_root = RB_ROOT;

struct dummy_struct tree_nodes[NODES];

void dump_rb_tree(struct rb_node *node, int level, enum child_dir state)
{
	struct dummy_struct *this;
	char prefix[40] = {0};
	int  i;

	if (!node)
		return;

	dump_rb_tree(node->rb_right, level+1, right_child);

	for (i = 0; i < level && i < 10; i++)
		strcat(prefix, "   ");

	if (state == left_child)
		printk(KERN_ERR "   %s| \n", prefix);
	this = rb_to_dummy(node);
	printk(KERN_ERR "%02d %s -%02d(%c)\n",
			level, prefix, this->idx,
			rb_is_red(node)?'r':'b');
	if (state == right_child)
		printk(KERN_ERR "   %s| \n", prefix);

	dump_rb_tree(node->rb_left, level+1, left_child);
}

static int insert_dummy_to_tree(struct dummy_struct *node)
{
	struct rb_node **new, *parent;

	new = &tree_root.rb_node;
	parent = NULL;

	while (*new) {
		struct dummy_struct *this;
		this = rb_to_dummy(*new);

		parent = *new;

		if (node->idx > this->idx)
			new = &((*new)->rb_right);
		else
			new = &((*new)->rb_left);
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color(&node->rb, &tree_root);

	return 0;
}

static int erase_dummy(struct dummy_struct *node)
{
	rb_erase(&node->rb, &tree_root);
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
	}
}

static int rb_tree_init(void)
{
	int i;
	struct rb_node *iter;
	struct dummy_struct *node;

	printk(KERN_ALERT "Hello, world \n");

	/* init the tree nodes */
	init();

	for(i = 0; i < NODES; i++)
		insert_dummy_to_tree(&tree_nodes[i]);

	dump_rb_tree(tree_root.rb_node, 0, root_node);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		//printk(KERN_ERR "%d \n", node->idx);
		iter = rb_next(iter);
	}

        printk(KERN_ALERT "Erase some nodes\n");
	for(i = 0; i < (NODES / 2); i++)
		erase_dummy(&tree_nodes[i]);
	dump_rb_tree(tree_root.rb_node, 0, root_node);

	return 0;
}
static void rb_tree_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(rb_tree_init);
module_exit(rb_tree_exit);
