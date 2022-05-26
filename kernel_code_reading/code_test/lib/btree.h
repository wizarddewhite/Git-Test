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
#define ORDER 6
#define PIVOT (ORDER/2)

struct btree_node {
	struct btree_node *parent;
	int parent_index;

	int used;
	int key[ORDER];
	void *data[ORDER];
	struct btree_node *children[ORDER+1];
};

#define is_leaf(node)	(!(node->children[0]))
#define is_low(node)  ((node->used < PIVOT))
#define is_empty(node)  ((node->used == 0))

struct btree {
	struct btree_node *root;
};
#define BTREE (struct btree) { NULL, }

struct btree_iterator {
	struct btree *tree;
	struct btree_node *node;
	int idx;
};

#define BTREE_START	((struct btree_node*)1UL)

#define BTREE_ITERATOR(name, btree)		\
	struct btree_iterator name = {		\
		.tree = btree,			\
		.node = BTREE_START,		\
	}

#define BTREE_ITERATOR_INIT(name)		\
		name.node = BTREE_START;

void panic(const char *fmt, ...);
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
void *btree_node_delete(struct btree_node *node, int idx, bool keep);
void btree_node_replace(struct btree_node *node, int idx, int key, void *data);
void *btree_delete(struct btree *tree, int key);
void rotate(struct btree_node *node, int idx);
void merge(struct btree_node *node, int idx);

struct btree_node *btree_first(struct btree_iterator *iter);
struct btree_node *btree_last(struct btree_iterator *iter);
struct btree_node *btree_next(struct btree_iterator *iter);
struct btree_node *btree_prev(struct btree_iterator *iter);

#define for_each_btree(iter)				\
	while (btree_next(iter))

#endif // __BTREE_H__
