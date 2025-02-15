#include <stdio.h>
#include <string.h>
#include "rb_tree.h"
#include "rbtree_augmented.h"
#include "interval_tree.h"

#define NODES 15

static struct rb_root_cached root = RB_ROOT_CACHED;
static struct interval_tree_node nodes[NODES];

#define rb_to_interval_node(X) rb_entry((X), struct interval_tree_node, rb)
void interval_node_print(struct rb_node *node, char *prefix, int level)
{
	struct interval_tree_node *this;

	this = rb_to_interval_node(node);
	printf("%02d %s (%c) [%lu, %lu] %lu\n",
			level, prefix,
			rb_is_red(node)?'r':'b',
			this->start, this->last, this->__subtree_last);
}

static void init(void)
{
	int i;

	for (i = 0; i < NODES; i++) {
		nodes[i].start = i * 100;
		nodes[i].last = i * 100 + 10;
	}

}

void insert_test()
{
	int i;
	struct interval_tree_node *node;

	init();

	for (i = 0; i < NODES; i++)
		interval_tree_insert(&nodes[i], &root);
	dump_rb_tree(root.rb_root.rb_node, 0, root_node, interval_node_print);

	for (node = interval_tree_iter_first(&root, 900, 1400); node;
	     node = interval_tree_iter_next(node, 900, 1400))
		printf("[%lu, %lu]\n", node->start, node->last);
}

int main()
{
	insert_test();

	return 0;
}
