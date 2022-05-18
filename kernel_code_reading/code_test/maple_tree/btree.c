#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "btree.h"

void panic(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printf("PANIC: ");
	printf(fmt, args);
	va_end(args);
	exit(1);
}

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

void dump_btree_node(struct btree_node *node, int level)
{
	int idx;

	if (!node)
		return;

	for (idx = 0; idx < node->used; idx++) {
		dump_btree_node(node->children[idx], level + 1);
		printf("%*s %d\n", level * 4, " ", node->key[idx]);
	}
	dump_btree_node(node->children[idx], level + 1);
}

void dump_btree(struct btree *tree)
{
	struct btree_node *node = tree->root;

	dump_btree_node(node, 0);
}

// insert key at [idx]
// return false means everything is fine, true means needs split
bool btree_node_insert(struct btree_node *node, int idx,
		struct btree_node *left, struct btree_node *right,
		int key, void *data)
{
	int i;
	if (node->used == ORDER)
		panic("node overflow\n");
	// shift right from idx
	for (i = node->used - 1; i >= idx; i--) {
		node->key[i+1] = node->key[i];
		node->data[i+1] = node->data[i];
	}
	for (i = node->used; i > idx; i--) {
		// shift parent_index too
		if (node->children[i])
			node->children[i]->parent_index++;
		node->children[i+1] = node->children[i];
	}
	node->key[idx] = key;
	node->data[idx] = data;

	// based on current split_node operation, left child is not changed
	if (left && !node->children[idx]) {
		node->children[idx] = left;
		left->parent = node;
		left->parent_index = idx;
	}
	if (right) {
		node->children[idx+1] = right;
		right->parent = node;
		right->parent_index = idx+1;
	}
	node->used++;

	return (node->used == ORDER);
}

void btree_insert(struct btree *tree, int key, void *data)
{
	struct btree_node *node = tree->root;
	struct btree_node *left = NULL, *right = NULL;
	int idx;

	// empty tree, create node and set it to root
	if (!node) {
		node = new_btree_node();
		tree->root = node;
	}

	do {
		// key already exist, update data only
		if (get_idx(node, key, &idx)) {
			node->data[idx] = data;
			return;
		}
	} while (node->children[idx] && (node = node->children[idx]));

	//printf("not found key %d, try to insert to %p\n", key, node);
	while (btree_node_insert(node, idx, left, right, key, data)) {
		// node is full, let's split it
		right = split_node(node, &key, &data);
		left = node;
		// insert the split key to parent
		node = node->parent;
		if (!node) {
			// we are the root,
			// allocate a new node and assign it to root
			node = new_btree_node();
			if (!node)
				panic("failed to allocate new node\n");
			tree->root = node;
		}
		get_idx(node, key, &idx);
	}
}

struct btree_node *split_node(struct btree_node *node, int *key, void **data)
{
	struct btree_node *right;
	int i, j;

	if (node->used != ORDER)
		panic("try to split a non-full node\n");

	right = new_btree_node();
	if (!right)
		panic("failed to allocate node on split\n");

	// assign middle one to key/data
	*key = node->key[PIVOT];
	*data = node->data[PIVOT];

	// assign right part to new node
	for (i = 0, j = PIVOT +1; j < ORDER; i++, j++) {
		right->key[i] = node->key[j];
		right->data[i] = node->data[j];
		right->children[i] = node->children[j];
	}
	right->children[i] = node->children[j];

	right->used = ORDER - PIVOT - 1;
	right->parent = node->parent;

	// clear right part in node
	node->used = PIVOT;
	return right;
}

void btree_first(struct btree_iterator *iter)
{
	// first entry in leftmost child
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;

	if (!node)
		node = tree->root;

	while (node && node->children[0]) {
		node = node->children[0];
	}

	iter->node = node;
	iter->idx = 0;
	return;
}

void btree_last(struct btree_iterator *iter)
{
	// last entry in rightmost child
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;

	if (!node)
		node = tree->root;

	while (node && node->children[node->used]) {
		node = node->children[node->used];
	}

	iter->node = node;
	iter->idx = node? node->used - 1:0;
	return;
}
