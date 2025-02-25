#include <stdio.h>
#include <string.h>
#include "rb_tree.h"
#include "rbtree_augmented.h"
#include "interval_tree.h"
#include "limits.h"

#define NODES 18

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

void iterator_test()
{
	int i;
	struct interval_tree_node *node;

	init();

	for (i = 0; i < NODES; i++)
		interval_tree_insert(&nodes[i], &root);
	dump_rb_tree(root.rb_root.rb_node, 0, root_node, interval_node_print);

	/* Iterate between [900, 1400] */
	printf("nodes overlap [900, 1400]\n");
	for (node = interval_tree_iter_first(&root, 900, 1400); node;
	     node = interval_tree_iter_next(node, 900, 1400))
		printf("[%lu, %lu]\n", node->start, node->last);

	printf("nodes overlap [1700, 1710]\n");
	/* Iterate just last node [1700, 1710] */
	for (node = interval_tree_iter_first(&root, 1700, 1710); node;
	     node = interval_tree_iter_next(node, 1700, 1710))
		printf("[%lu, %lu]\n", node->start, node->last);
}


void insert_duplicate_test()
{
	int i;
	struct interval_tree_node *node;

	init();

	/* we force nodes[4] has the same value as nodes[5] */
	printf("[%lu, %lu] -> [%lu, %lu]\n",
			nodes[4].start, nodes[4].last,
			nodes[5].start, nodes[5].last);
	nodes[4].start = nodes[5].start;
	nodes[4].last = nodes[5].last;

	/* Now let's expand nodes[2].last to more than nodes[17].last*/
	nodes[2].last = nodes[NODES-1].last + 1;

	for (i = 0; i < NODES; i++)
		interval_tree_insert(&nodes[i], &root);
	dump_rb_tree(root.rb_root.rb_node, 0, root_node, interval_node_print);

	for (node = interval_tree_iter_first(&root, 300, 600); node;
	     node = interval_tree_iter_next(node, 300, 600))
		printf("[%lu, %lu]\n", node->start, node->last);
}

void span_iterator()
{
	int i;
	struct interval_tree_node *node;
	struct interval_tree_span_iter allowed_span;

	init();

	for (i = 0; i < NODES; i++)
		interval_tree_insert(&nodes[i], &root);
	dump_rb_tree(root.rb_root.rb_node, 0, root_node, interval_node_print);

	i = 0;
	interval_tree_for_each_span(&allowed_span, &root, 0, ULONG_MAX) {
		printf("[%d]: [%lu, %lu] %s\n", i++,
				allowed_span.start_hole, allowed_span.last_hole,
				allowed_span.is_hole ? "hole":"used");
	}
}

int main()
{
	// insert_test();
	// iterator_test();
	// insert_duplicate_test();
	span_iterator();

	return 0;
}
