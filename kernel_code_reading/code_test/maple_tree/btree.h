#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdbool.h>

/* 
 * At most one node with ORDER could store (ORDER - 1) number of key
 *
 * But we allocate ORDER number of key and (ORDER + 1) number of pointer for
 * split.
 */
#define ORDER 5

struct btree_node {
	struct btree_node *parent;
	int parent_index;

	int used;
	int key[ORDER];
	void *data[ORDER];
	struct btree_node *children[ORDER+1];
};

struct btree {
	struct btree_node *root;
};
#define BTREE (struct btree) { NULL, }

bool get_idx(struct btree_node *node, int key, int *index);
void *btree_lookup(struct btree *tree, int key);
struct btree_node *new_btree_node();
#endif // __BTREE_H__
