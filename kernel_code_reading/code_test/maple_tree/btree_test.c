#include <stdio.h>
#include "btree.h"

void getidx_in_node()
{
	int index;
	struct btree_node node;

	node.key[0] = 22;
	node.key[1] = 33;
	node.used = 2;

	if (get_idx(&node, 22, &index))
		printf("key %d at index %d\n", 22, index);
	if (!get_idx(&node, 23, &index))
		printf("key %d not exist before %d\n", 23, index);
	if (!get_idx(&node, 43, &index))
		printf("key %d not exist before %d\n", 43, index);

	node.key[2] = 42;
	node.key[3] = 53;
	node.used = 4;
	if (!get_idx(&node, 43, &index))
		printf("key %d not exist before %d\n", 43, index);
	if (get_idx(&node, 53, &index))
		printf("key %d at index %d\n", 53, index);
}

void lookup_key()
{
	struct btree tree = BTREE;
	struct btree_node *node;
	int key[] = {22, 33, 10, 15};
	int index;
	void *data;

	data = btree_lookup(&tree, key[0]);
	if (data)
		printf("Found key %d with data %p\n", key[0], data);

	node = new_btree_node();
	if (!node)
		return;
	// assign it to root
	tree.root = node;
	node->key[0] = key[0];
	node->data[0] = &key[0];
	node->key[1] = key[1];
	node->data[1] = &key[1];
	node->used = 2;

	data = btree_lookup(&tree, key[0]);
	if (data)
		printf("Found key %d with data %p: %p\n", key[0], data, &key[0]);

	node = new_btree_node();
	if (!node)
		return;
	// assign it to root's left most child
	tree.root->children[0] = node;
	node->key[0] = key[2];
	node->data[0] = &key[2];
	node->key[1] = key[3];
	node->data[1] = &key[3];
	node->used = 2;
	data = btree_lookup(&tree, key[2]);
	if (data)
		printf("Found key %d with data %p: %p\n", key[2], data, &key[2]);

	node = new_btree_node();
	if (!node)
		return;
	// assign it to root's second
	tree.root->children[1] = node;
	node->key[0] = 24;
	node->key[1] = 30;
	node->used = 2;

	node = new_btree_node();
	if (!node)
		return;
	// assign it to root's third
	tree.root->children[2] = node;
	node->key[0] = 40;
	node->key[1] = 52;
	node->used = 2;

	dump_btree(&tree);
}

void insert_to_node() 
{
	int i, idx;
	int key[] = {22, 10, 33, 15, 45};
	struct btree_node *node = new_btree_node();
	void *data;
	if (!node)
		return;

	for (i = 0; i < 5; i++) {
		get_idx(node, key[i], &idx);
		printf("key: %d may at idx %d\n", key[i], idx);
		btree_node_insert(node, idx, NULL, key[i], NULL);
		dump_btree_node(node, 0);
	}

	node = split_node(node, &i, &data);
	printf("dump split right node:\n");
	dump_btree_node(node, 0);
}

int main()
{
	// getidx_in_node();
	// lookup_key();
	// insert_key();
	insert_to_node();
}
