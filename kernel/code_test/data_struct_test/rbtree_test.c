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
#include "rbtree_augmented.h"

#define NODES       15

struct dummy_struct {
	int idx;
	struct rb_node rb;
};
#define rb_to_dummy(X) rb_entry((X), struct dummy_struct, rb)

static struct rb_root tree_root;
struct dummy_struct tree_nodes[NODES];

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

void dummy_print(struct rb_node *node, char *prefix, int level)
{
	struct dummy_struct *this;

	this = rb_to_dummy(node);
	printf("%02d %s -%02d(%c)\n",
			level, prefix, this->idx,
			rb_is_red(node)?'r':'b');
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


/* borrow from lib/rbtree_test.c */
struct test_node {
        int key;
	struct rb_node rb;

	/* following fields used for testing augmented rbtree functionality */
	int val;
	int augmented;
};
#define rb_to_test_node(X) rb_entry((X), struct test_node, rb)

static struct rb_root_cached cached_root = RB_ROOT_CACHED;
static struct test_node cached_nodes[NODES];

static void init_cached(void)
{
	int i;
	for(i = 0; i < NODES; i++) {
		cached_nodes[i].key = i;
		cached_nodes[i].val = i * 10;
	}
}

void test_node_print(struct rb_node *node, char *prefix, int level)
{
	struct test_node *this;

	this = rb_to_test_node(node);
	printf("%02d %s -%02d(%c)\n",
			level, prefix, this->key,
			rb_is_red(node)?'r':'b');
}

void test_node_print_augmented(struct rb_node *node, char *prefix, int level)
{
	struct test_node *this;

	this = rb_to_test_node(node);
	printf("%02d %s -%02d(%c) %d:%d\n",
			level, prefix, this->key,
			rb_is_red(node)?'r':'b',
			this->val, this->augmented);
}


void insert_cached(struct test_node *node, struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *parent = NULL;
	int key = node->key;
	bool leftmost = true;

	while (*new) {
		parent = *new;
		if (key < rb_entry(parent, struct test_node, rb)->key)
			new = &parent->rb_left;
		else {
			new = &parent->rb_right;
			leftmost = false;
		}
	}

	rb_link_node(&node->rb, parent, new);
	rb_insert_color_cached(&node->rb, root, leftmost);
}

#define NODE_VAL(node) ((node)->val)

RB_DECLARE_CALLBACKS_MAX(static, augment_callbacks,
			 struct test_node, rb, int, augmented, NODE_VAL)

static void insert_augmented(struct test_node *node,
			     struct rb_root_cached *root)
{
	struct rb_node **new = &root->rb_root.rb_node, *rb_parent = NULL;
	int key = node->key;
	int val = node->val;
	struct test_node *parent;

	while (*new) {
		rb_parent = *new;
		parent = rb_entry(rb_parent, struct test_node, rb);
		if (parent->augmented < val)
			parent->augmented = val;
		if (key < parent->key)
			new = &parent->rb.rb_left;
		else
			new = &parent->rb.rb_right;
	}

	node->augmented = val;
	rb_link_node(&node->rb, rb_parent, new);
	rb_insert_augmented(&node->rb, &root->rb_root, &augment_callbacks);
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

	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		printf("%d: %p\n", node->idx, node);
		iter = rb_next(iter);
	}
}

/* insert with same value */
void insert_test2()
{
	int i;
	struct rb_node *iter;
	struct dummy_struct *node;
	init();
	/* init the tree nodes */
	for(i = 0; i < NODES - 5; i++) {
		/* force to have two 6 value */
		if (i == 7)
			tree_nodes[i].idx = 6;
		insert_dummy_to_tree(&tree_nodes[i]);
	}

	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		printf("%d: %p\n", node->idx, node);
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

	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);

	iter = rb_first(&tree_root);
	while (iter) {
		node = rb_to_dummy(iter);
		printf("%d \n", node->idx);
		iter = rb_next(iter);
	}
}

void erase_test()
{
	int i;
	struct rb_node *iter;
	struct dummy_struct *node;
	init();
	/* init the tree nodes */
	for(i = 0; i < NODES; i++) {
		insert_dummy_to_tree(&tree_nodes[i]);
	}

	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);

	printf("Erase element %d\n", 19);
	rb_erase(&tree_nodes[19].rb, &tree_root);
	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);

	printf("Erase element %d\n", 11);
	rb_erase(&tree_nodes[11].rb, &tree_root);
	dump_rb_tree(tree_root.rb_node, 0, root_node, dummy_print);
}

void insert_cached_test()
{
	int i;
	struct rb_node *iter;
	struct test_node *node;

	init_cached();
	for (i = 0; i < NODES; i++) {
		insert_cached(&cached_nodes[i], &cached_root);
	}

	dump_rb_tree(cached_root.rb_root.rb_node, 0, root_node, test_node_print);

	iter = rb_first(&cached_root.rb_root);
	while (iter) {
		node = rb_to_test_node(iter);
		printf("%d ", node->key);
		iter = rb_next(iter);
	}
	printf("\n");

	node = rb_to_test_node(cached_root.rb_leftmost);
	printf("The left most entry %d:%p\n", node->key, node);
}

void insert_augmented_test()
{
	int i;
	struct rb_node *iter;
	struct test_node *node;

	init_cached();

	for (i = 0; i < NODES; i++)
		insert_augmented(&cached_nodes[i], &cached_root);
	dump_rb_tree(cached_root.rb_root.rb_node, 0, root_node, test_node_print_augmented);

}

void insert_augmented_duplicated_test()
{
	int i;
	struct rb_node *iter;
	struct test_node *node;

	init_cached();

	/* Force node[5] equals to node[4] */
	cached_nodes[5].key = cached_nodes[4].key;
	cached_nodes[5].val = cached_nodes[4].val;

	for (i = 0; i < NODES; i++)
		insert_augmented(&cached_nodes[i], &cached_root);
	dump_rb_tree(cached_root.rb_root.rb_node, 0, root_node, test_node_print_augmented);

}

int main()
{
	// insert_test();
	// insert_test2();
	// case2_verify();
	// erase_test();

	// insert_cached_test();
	insert_augmented_duplicated_test();

	return 0;
}
