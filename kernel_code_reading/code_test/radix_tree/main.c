#include <stdio.h>
#include "radix_tree.h"

struct item {
	int index;
};

static struct item *items;
static RADIX_TREE(rx_tree, 0/* GFP_KERNEL */);
int num = 17;

void dump_radix_tree(struct radix_tree_node *node, int level)
{
	int  i;

	if (!node)
		return;

	/* This is a leaf, so print the index */
	if (!radix_tree_is_internal_node(node)) {
		struct item *iter = (struct item *)node;
		printf("%4d\n", iter->index);
		return;
	}

	/* Neither a leaf nor the root */
	if (entry_to_node(node)->parent)
		printf("+(s:%d, b:%d)\n",
			entry_to_node(node)->parent->shift,
			(1 << entry_to_node(node)->parent->shift) *
			entry_to_node(node)->offset
			);

	for (i = 0; i < RADIX_TREE_MAP_SIZE; i++) {
		if (!level)
			printf("|[%d]", i);
		else
			printf("%*s|[%d]", level*4, " ", i);
		if (entry_to_node(node)->slots[i])
			dump_radix_tree(entry_to_node(node)->slots[i], level+1);
		else
			printf("\n");
	}
}

int main()
{
	int i;

	items = malloc(sizeof(struct item) * num);
	if (!items)
		return 0;

	for (i = 0; i < num; i++) {
		items[i].index = i;
		if (i == 2)
			continue;
		radix_tree_insert(&rx_tree, i, &items[i]);
	}
	dump_radix_tree(rx_tree.rnode, 0);
}
