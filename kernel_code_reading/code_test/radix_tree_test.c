#include <stdio.h>
#include "radix_tree.h"

struct item {
	int index;
};

static struct item *items;
static RADIX_TREE(rx_tree, 0/* GFP_KERNEL */);
int num = 17;

static unsigned long get_offset(struct radix_tree_node *node)
{
	struct radix_tree_node *parent;
	unsigned long offset = 0;

	parent = node->parent;

	while (parent) {
		offset += ((unsigned long)(node->offset) << (unsigned long)parent->shift);
		node = parent;
		parent = node->parent;
	}

	return offset;
}

void dump_radix_tree(struct radix_tree_node *parent, struct radix_tree_node *node, int level,
		bool supress, int index)
{
	int  i;

	if (!node)
		return;

	/* This is a leaf, so print the index */
	if (!radix_tree_is_internal_node(node)) {
		struct item *iter = (struct item *)node;
		unsigned long offset = parent ? get_offset(parent) + index :0;
		printf("    %016lx: %p\n", offset, iter);
		return;
	}

	node = entry_to_node(node);

	/*
	 * Neither a leaf nor the root
	 * 1. leaf return above
	 * 2. root doesn't have parent
	 */
	if (node->parent)
		printf("+(s:%d, o:%016lx, c: %2d)\n",
			node->parent->shift,
			get_offset(node), node->count
			);

	for (i = 0; i < RADIX_TREE_MAP_SIZE; i++) {
		if (supress && !node->slots[i])
			continue;

		if (!level)
			printf("|[%02d]", i);
		else
			printf("%*s|[%02d]", level*4, " ", i);
		if (node->slots[i])
			dump_radix_tree(node, node->slots[i], level+1, supress, i);
		else
			printf("\n");
	}
}

void small_test()
{
	int i;

	items = malloc(sizeof(struct item) * num);
	if (!items)
		return;

	for (i = 0; i < num; i++) {
		items[i].index = i;
		if (i == 2)
			continue;
		radix_tree_insert(&rx_tree, i, &items[i]);
	}
	radix_tree_insert(&rx_tree, 0xff, &items[2]);
	dump_radix_tree(NULL, rx_tree.rnode, 0, false, 0);
}

void large_test()
{
	int i;

	items = malloc(sizeof(struct item) * num);
	if (!items)
		return;

	for (i = 0; i < num; i++) {
		items[i].index = i;
	}
	//radix_tree_insert(&rx_tree, 0xffffffff, &items[i]);
	radix_tree_insert(&rx_tree, 0xf000000000000000, &items[i]);
	radix_tree_insert(&rx_tree, 0xffffffffffffffff, &items[i]);
	dump_radix_tree(NULL, rx_tree.rnode, 0, true, 0);
}

void lookup_delete_test()
{
	int i;
	struct item *item;

	items = malloc(sizeof(struct item) * num);
	if (!items)
		return;

	for (i = 0; i < 10; i++) {
		items[i].index = i;
		if (i == 2)
			continue;
		radix_tree_insert(&rx_tree, i, &items[i]);
	}
	dump_radix_tree(NULL, rx_tree.rnode, 0, false, 0);

	for (i = 0; i < 10; i++) {
		if (i == 2)
			continue;
		item = radix_tree_lookup(&rx_tree, i);
		BUG_ON(item != &items[i]);
	}

	item = radix_tree_lookup(&rx_tree, 2);
	BUG_ON(item);

	item = radix_tree_lookup(&rx_tree, 0);
	printf("%d's item is %p\n", 0, item);
	radix_tree_delete(&rx_tree, 0);
	item = radix_tree_lookup(&rx_tree, 0);
	if (!item)
		printf("%d's is removed\n", 0);
}

int main()
{
	// small_test();
	// large_test();
	//idr_test();
	lookup_delete_test();
}
