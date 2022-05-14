#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "btree.h"

struct btree_node *new_btree_node()
{
	struct btree_node *node;

	node = malloc(sizeof(struct btree_node));
	if (!node)
		return NULL;
	memset(node, 0, sizeof(struct btree_node));
	return node;
}

/* 
 * set index to first one equal or bigger than key 
 * return true means exact match, false for not.
 */ 
bool get_idx(struct btree_node *node, int key, int *index)
{
	int i;

	*index = node->used;
	for (i = 0; i < node->used; i++) {
		if (node->key[i] < key)
			continue;

		*index = i;
		if (node->key[i] == key)
			return true;
		else
			return false;
	}

	return false;
}

void *btree_lookup(struct btree *tree, int key)
{
	struct btree_node *node = tree->root;
	int index;

	while (node) {
		if (get_idx(node, key, &index))
			return node->data[index];

		node = node->children[index];
	}
	return NULL;
}
