#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "btree.h"

void panic(const char *fmt, ...)
{
	va_list args;
	printf("PANIC: ");
	va_start(args, fmt);
	vprintf(fmt, args);
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

bool __btree_lookup(struct btree_iterator *iter, int key)
{
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;

	if (iter->node == BTREE_START)
		iter->node = tree->root;

	while (iter->node) {
		if (get_idx(iter->node, key, &iter->idx)) {
			return true;
		}

		iter->node = iter->node->children[iter->idx];
	}
	return false;
}

void *btree_lookup(struct btree *tree, int key)
{
	BTREE_ITERATOR(biter, tree);
	return __btree_lookup(&biter, key) ?
		biter.node->data[biter.idx] : NULL;
}

void dump_btree_node(struct btree_node *node, int level)
{
	int idx;

	if (!node)
		return;

#ifdef DEBUG
	printf("%*s %p -> %p(%d)\n", level * 4, " ",
			node, node->parent, node->parent_index);
#endif

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

	// adjust children's parent/parent_index in right
	for (i = 0; i <= right->used; i++) {
		if (right->children[i]) {
			right->children[i]->parent_index = i;
			right->children[i]->parent = right;
		}
	}

	// clear right part in node
	node->used = PIVOT;
	return right;
}

struct btree_node *btree_first(struct btree_iterator *iter)
{
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;

	if (node == BTREE_START)
		node = tree->root;

	// first entry in leftmost child
	while (node && node->children[0])
		node = node->children[0];

	iter->node = node;
	iter->idx = 0;
	return node;
}

struct btree_node *btree_last(struct btree_iterator *iter)
{
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;

	if (node == BTREE_START)
		node = tree->root;

	// last entry in rightmost child
	while (node && node->children[node->used])
		node = node->children[node->used];

	iter->node = node;
	iter->idx = node? node->used - 1:0;
	return node;
}

struct btree_node *btree_next(struct btree_iterator *iter)
{
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;
	struct btree_node *parent;

	if (node == BTREE_START) {
		btree_first(iter);
		goto out;
	}

	// leftmost child if has right child
	if (node->children[iter->idx+1]) {
		node = node->children[iter->idx+1];
		while (node->children[0])
			node = node->children[0];

		iter->node = node;
		iter->idx = 0;
		goto out;
	}

	// printf("iter index: %d\n", iter->idx);
	// or next sibling
	if (iter->idx + 1 < node->used) {
		iter->idx++;
		goto out;
	}

	// or the first ancestor who's child is left one
	while (node && node->parent) {
		parent = node->parent;

		//printf(" node(%p:%d) parent(%p:%d) index: %d\n",
		//	node, node->used, parent, parent->used, node->parent_index);
		if (node->parent_index < parent->used) {
			iter->node = parent;
			iter->idx = node->parent_index;
			goto out;
		}
		node = parent;
	}

	iter->node = NULL;
out:
	return iter->node;
}

struct btree_node *btree_prev(struct btree_iterator *iter)
{
	struct btree *tree = iter->tree;
	struct btree_node *node = iter->node;
	struct btree_node *parent;

	if (node == BTREE_START) {
		btree_last(iter);
		goto out;
	}

	// rightmost child if has left child
	if ((iter->idx >= 0) && node->children[iter->idx]) {
		node = node->children[iter->idx];
		while (node->children[node->used])
			node = node->children[node->used];

		iter->node = node;
		iter->idx = node->used - 1;
		goto out;
	}

	// or previous sibling
	if (iter->idx > 0) {
		iter->idx--;
		goto out;
	}

	// or the first ancestor who's child is right one
	while (node && node->parent) {
		parent = node->parent;

		//printf(" node(%p:%d) parent(%p:%d) index: %d\n",
		//	node, node->used, parent, parent->used, node->parent_index);
		if (node->parent_index > 0) {
			iter->node = parent;
			iter->idx = node->parent_index - 1;
			goto out;
		}
		node = parent;
	}

	iter->node = NULL;
out:
	return iter->node;
}

void *btree_node_delete(struct btree_node *node, int idx)
{
	int i;
	void *data;

	if (idx >= node->used)
		panic("Deleting out of range\n");

	data = node->data[idx];

	// shift left from idx + 1
	for (i = idx; i < node->used - 1; i++) {
		node->key[i] = node->key[i+1];
		node->data[i] = node->data[i+1];
	}
	node->used--;
	return data;
}

void btree_node_replace(struct btree_node *node, int idx, int key, void *data)
{
	if (idx >= node->used)
		panic("replace idx out of range\n");

	node->key[idx] = key;
	node->data[idx] = data;
}

/*
 * There are four cases on deletion:
 * 1. In leaf node, just delete it
 * 2. Replace key/data with next, which must be in leaf. Then remove next and
 *    node point to next.
 * 3. If node->used < PIVOT
 *    a. if sibling->used >= PIVOT, rotate at parent
 *    b. else, merge with sibling
 */
void *btree_delete(struct btree *tree, int key)
{
	struct btree_node *node, *parent, *sibling = NULL;
	int index;
	void *data;
	BTREE_ITERATOR(biter, tree);

	if (!__btree_lookup(&biter, key))
		return NULL;

	// case 1 & 2
	if (!is_leaf(biter.node)) {
		// case 2
		node = biter.node;
		index = biter.idx;
		btree_next(&biter);
		btree_node_replace(node, index,
			biter.node->key[biter.idx], biter.node->data[biter.idx]);
	}

	data = btree_node_delete(biter.node, biter.idx);

	if (!is_low(biter.node))
		goto out;

	// case 3
	node = biter.node;
	index = biter.idx;
	parent = node->parent;

	if (!parent)
		goto out;
	if (node->parent_index+1 <= parent->used) // !node->parent_index
		sibling = parent->children[node->parent_index+1];
	if (!sibling)
		goto out;

	// 3.a
	if (!is_low(sibling))
		rotate(parent, node->parent_index);
	else // 3.b
		merge();

out:
	return NULL;
}

void rotate(struct btree_node *node, int idx)
{
      struct btree_node *left, *right;

      left = node->children[idx];
      right = node->children[idx+1];

      // insert idx to left
      btree_node_insert(left, left->used, NULL, right->children[0],
                      node->key[idx], node->data[idx]);

      // replace idx with right[0]
      btree_node_replace(node, idx, right->key[0], right->data[0]);

      // remove right[0]
      btree_node_delete(right, 0);
}

void merge()
{}
