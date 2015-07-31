/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/06/2015 11:11:05 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include "rb_tree.h"

enum child_dir {
	left_child,
	right_child,
	root_node
};

struct dummy_struct {
	int idx;
	struct rb_node rb;
};

#define rb_to_dummy(X) rb_entry((X), struct dummy_struct, rb)
#define NODES       7

static struct rb_root tree_root;

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
		printf("   %s| \n", prefix);
	this = rb_to_dummy(node);
	printf("%02d %s -%02d(%c)\n",
			level, prefix, this->idx,
			rb_is_red(node)?'r':'b');
	if (state == right_child)
		printf("   %s| \n", prefix);

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

void insert_test()
{
	int i;
	struct rb_node *iter;
	struct dummy_struct *node;
	init();
	/* init the tree nodes */
	for(i = 0; i < NODES; i++) {
		insert_dummy_to_tree(&tree_nodes[i]);
	}

	dump_rb_tree(tree_root.rb_node, 0, root_node);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		printf("%d \n", node->idx);
		iter = rb_next(iter);
	}
}

void case2_verify()
{
	int i;
	struct rb_node *iter;
	struct dummy_struct *node;
	init();
	/* init the tree nodes */
	insert_dummy_to_tree(&tree_nodes[5]);
	insert_dummy_to_tree(&tree_nodes[6]);
	insert_dummy_to_tree(&tree_nodes[2]);
	rb_set_parent_color(&tree_nodes[6].rb, &tree_nodes[5].rb, RB_BLACK);
	insert_dummy_to_tree(&tree_nodes[4]);

	dump_rb_tree(tree_root.rb_node, 0, root_node);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		printf("%d \n", node->idx);
		iter = rb_next(iter);
	}
}

int main()
{
	case2_verify();

	return 0;
}
