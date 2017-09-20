#include <stdio.h>
#include "radix_tree.h"

struct item {
	int index;
};

static struct item *items;
static RADIX_TREE(rx_tree, 0/* GFP_KERNEL */);

int main()
{
	int i;
	printf("Hello World\n");

	items = malloc(sizeof(struct item) * 5);
	if (!items)
		return 0;

	for (i = 0; i < 5; i++) {
		items[i].index = i;
		radix_tree_insert(&rx_tree, i, &items[i]);
	}
}
