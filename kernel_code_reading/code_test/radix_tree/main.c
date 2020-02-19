#include <stdio.h>
#include "radix_tree.h"
#include "idr.h"

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

void dump_radix_tree(struct radix_tree_node *node, int level,
		bool supress, int index)
{
	int  i;

	if (!node)
		return;

	/* This is a leaf, so print the index */
	if (!radix_tree_is_internal_node(node)) {
		struct item *iter = (struct item *)node;
		printf("%4d(%x)\n", iter->index, index);
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
			dump_radix_tree(node->slots[i], level+1, supress, i);
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
	dump_radix_tree(rx_tree.rnode, 0, false, 0);
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
	dump_radix_tree(rx_tree.rnode, 0, true, 0);
}

void lookup_delete_test()
{
	int i;
	struct item *item;

	items = malloc(sizeof(struct item) * num);
	if (!items)
		return;

	for (i = 0; i < 1; i++) {
		items[i].index = i;
		if (i == 2)
			continue;
		radix_tree_insert(&rx_tree, i, &items[i]);
	}
	dump_radix_tree(rx_tree.rnode, 0, false, 0);

	item = radix_tree_lookup(&rx_tree, 2);
	if (item)
		printf("Found\n");
	else
		printf("Not Found\n");

	radix_tree_delete(&rx_tree, 0);
}

void idr_test()
{
	struct idr idr_index;
	int index;

	items = malloc(sizeof(struct item) * num);

	idr_init(&idr_index);

	index = idr_alloc(&idr_index, &items[1], 0, 10, (gfp_t)1);
	printf("index is %d\n", index);

}

int main()
{
	//small_test();
	//large_test();
	idr_test();
}
