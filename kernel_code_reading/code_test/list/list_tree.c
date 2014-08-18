/*
 * =====================================================================================
 *
 *       Filename:  list_tree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/18/2014 09:26:34 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "list.h"

struct list_tree {
	int index;
	struct list_tree *parent;
	struct list_head list;
	struct list_head child;
};

struct list_tree nodes[30];

void node_init(struct list_tree *node, int val)
{
	if (!node)
		return;

	node->parent = NULL;
	node->index = val;
	INIT_LIST_HEAD(&node->list);
	INIT_LIST_HEAD(&node->child);
}

typedef void *(*traverse_lt_func)(struct list_tree *me, void *data);
void *traverse_list_tree(struct list_tree *start, traverse_lt_func pre,
		void *data)
{
	struct list_tree *nd, *n_nd;
	void *ret;

	/* We started with a tree node, iterate all childs */
	for (nd = list_first_entry_or_null(&start->child, struct list_tree, list);
		nd; nd = n_nd) {

		if (pre && ((ret = pre(nd, data)) != NULL))
			return ret;

		/* If we have child, go down */
		if ((n_nd = list_first_entry_or_null(&nd->child, struct list_tree, list)))
			/* Depth first...do children */
			continue;

		/* ok, try next sibling instead. */
		if (nd->list.next != &nd->parent->child) {
			n_nd = list_next_entry(nd, struct list_tree, list);
			continue;
		} else {
			n_nd = nd;
			/* Walk up to next valid sibling. */
			do {
				n_nd = n_nd->parent;
				if (n_nd == start)
					return NULL;
			} while (n_nd->list.next == &n_nd->parent->child);
			n_nd = list_next_entry(n_nd, struct list_tree, list);
		}
	}
	return NULL;
}

void *show(struct list_tree *me, void *data)
{
	printf("node: %d\n", me->index);
	return NULL;
}

int main()
{
	int i;
	
	for (i = 0; i < 30; i++)
		node_init(&nodes[i], i);
	//for (i = 0; i < 30; i++)
	//	printf("node: %d\n", nodes[i].index);

	/* 0 has child 1, 2 */
	list_add_tail(&nodes[1].list, &nodes[0].child);
	list_add_tail(&nodes[2].list, &nodes[0].child);
	nodes[1].parent = &nodes[0];
	nodes[2].parent = &nodes[0];

	/* 1 has child 3, 4, 5 */
	list_add_tail(&nodes[3].list, &nodes[1].child);
	list_add_tail(&nodes[4].list, &nodes[1].child);
	list_add_tail(&nodes[5].list, &nodes[1].child);
	nodes[3].parent = &nodes[1];
	nodes[4].parent = &nodes[1];
	nodes[5].parent = &nodes[1];

	/* 2 has child 6, 7 */
	list_add_tail(&nodes[6].list, &nodes[2].child);
	list_add_tail(&nodes[7].list, &nodes[2].child);
	nodes[6].parent = &nodes[2];
	nodes[7].parent = &nodes[2];

	/* 6 has child 8, 9 */
	list_add_tail(&nodes[8].list, &nodes[6].child);
	list_add_tail(&nodes[9].list, &nodes[6].child);
	nodes[8].parent = &nodes[6];
	nodes[9].parent = &nodes[6];

	traverse_list_tree(&nodes[0], show, NULL);

	return 0;
}
