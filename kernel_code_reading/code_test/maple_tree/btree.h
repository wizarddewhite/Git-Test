#ifndef __BTREE_H__
#define __BTREE_H__

#include <stdbool.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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
void dump_btree_node(struct btree_node *node, int level);
void dump_btree(struct btree *tree);
bool btree_node_insert(struct btree_node *node, int idx,
		struct btree_node *left, struct btree_node *right,
		int key, void *data);
void btree_insert(struct btree *tree, int key, void *data);
struct btree_node *split_node(struct btree_node *node, int *key, void **data);
#endif // __BTREE_H__
