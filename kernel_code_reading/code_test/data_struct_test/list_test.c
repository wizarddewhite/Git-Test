#include <stdio.h>
#include "list.h"

struct list_dummy {
	struct list_head list_node;
	int index;
};

typedef struct list_dummy list_dummy;

void list_del_test();
void list_add_tail_test();
void list_for_each_entry_safe_test();
void list_for_each_entry_test(int create);

#define FIRST_LENGTH 11
LIST_HEAD(test_list);
struct list_dummy elem[FIRST_LENGTH];

#define SECOND_LENGTH 2
LIST_HEAD(test_list2);
struct list_dummy elem2[SECOND_LENGTH];

void isempty_test()
{
	if (list_empty(&test_list))
		printf("list is empty\n");
	else
		printf("list is non empty\n");
}

void islast_test()
{
	/* list_head is both first and last element, if it is am empty.*/
	if (list_empty(&test_list))
		printf("list is empty\n");
	if (list_is_last(&test_list, &test_list))
		printf("head is last\n");
	else
		printf("head is not last\n");
	if (list_is_first(&test_list, &test_list))
		printf("head is first too\n");
	else
		printf("head is not first\n");
}

void isfirst_test()
{
	struct list_dummy *elem;

	list_add_tail_test();

	elem = list_first_entry(&test_list, struct list_dummy, list_node);
	printf("index of first node is %d\n", elem->index);

	if (list_is_last(&test_list, &test_list))
		printf("head is last\n");
	else
		printf("head is not last\n");
	if (list_is_first(&elem->list_node, &test_list))
		printf("elem is first\n");
	else
		printf("elem is non first\n");
}

void list_add_test()
{
	int i;
	struct list_dummy *iter;

	for (i = 0; i < 10; i++)
	{
		elem[i].index = i;
		list_add(&elem[i].list_node, &test_list);
	}

	printf("=== After list add ===\n");
	list_for_each_entry(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

void list_del_test()
{
	printf("prepare the list for test: \n");
	list_add_tail_test();
	// list_for_each_entry_safe_test(0);

	list_del(&elem[7].list_node);
	printf("=== After delete the elment 7: ===\n");
	list_for_each_entry_safe_test(0);
}

void list_add_tail_test()
{
	int i;
	struct list_dummy *iter;

	for (i = 1; i < FIRST_LENGTH; i++)
	{
		elem[i].index = i;
		list_add_tail(&elem[i].list_node, &test_list);
	}

	printf("=== After list add tail ===\n");
	list_for_each_entry(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

void list_for_each_test()
{
	struct list_head *iter;
	int i = 0;

	for (i = 0; i < FIRST_LENGTH; i++)
	{
		elem[i].index = i;
		list_add_tail(&elem[i].list_node, &test_list);
	}

	i = 0;

	list_for_each(iter, &test_list) {
		struct list_dummy *entry = list_entry(iter, struct list_dummy, list_node);
		printf("%d %p(%d) %p(%d)\n", i, iter, entry->index,
			&elem[i], elem[i].index);
		i++;
	}
}

void list_for_each_entry_test(int create)
{
	struct list_dummy *iter;

	if (create)
		list_add_tail_test();
}

void list_first_entry_test()
{
	struct list_dummy *iter;

	list_add_tail_test();

	iter = list_first_entry(&test_list, struct list_dummy, list_node);

	printf("index of first node is %d\n", iter->index);
}

void list_next_entry_test()
{
	int i;
	struct list_dummy *iter;

	list_add_tail_test();

	iter = list_first_entry(&test_list, struct list_dummy, list_node);
	printf("index of first node is %d\n", iter->index);


	i = 2;
	while(1) {

		iter = list_next_entry(iter, struct list_dummy, list_node);
		printf("index of %dth node is %d\n", i++, iter->index);

		if (iter->list_node.next == &test_list) {
			printf("The end of the list, no more entry\n");
			break;
		}
	}

}

void list_for_each_entry_reverse_test()
{
	struct list_dummy *iter;

	list_add_tail_test();

	printf("=== Reverse order ===\n");
	list_for_each_entry_reverse(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}

}

void list_for_each_entry_safe_test(int create)
{
	struct list_dummy *iter;
	struct list_dummy *next;

	if (create)
		list_add_tail_test();

	printf("=== list for each safe ===\n");
	list_for_each_entry_safe(iter, next, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

void list_move_test()
{
	struct list_dummy *iter, *iter2;
	struct list_dummy *tmp;

	list_add_tail_test();

	printf("---   we want to move 5 after 8\n");
	list_for_each_entry_safe(iter, tmp, &test_list, list_node) {
		if (iter->index == 5) {
			list_for_each_entry(iter2, &test_list, list_node) {
				if (iter2->index == 8)
					list_move(&iter->list_node,
						  &iter2->list_node);
			}
		}
	}

	list_for_each_entry(iter, &test_list, list_node)
		printf("index of entry is %d\n", iter->index);
}

void list_move_tail_test()
{
	struct list_dummy *iter, *iter2;
	struct list_dummy *tmp;

	list_add_tail_test();

	printf("---   we want to move 5 to tail\n");
	list_for_each_entry_safe(iter, tmp, &test_list, list_node) {
		if (iter->index == 5) {
			list_move_tail(&iter->list_node, &test_list);
		}
	}

	list_for_each_entry(iter, &test_list, list_node)
		printf("index of entry is %d\n", iter->index);
}

void list_splice_tail_test()
{
	int i;
	struct list_dummy *iter;

	for (i = 0; i < SECOND_LENGTH; i++)
	{
		elem2[i].index = i + 100;
		list_add_tail(&elem2[i].list_node, &test_list2);
	}

	printf("=== After list add test_list2 ===\n");
	list_for_each_entry(iter, &test_list2, list_node) {
		printf("index of entry is %d\n", iter->index);
	}

	list_add_tail_test();

	list_splice_tail(&test_list, &test_list2);

	printf("--- merge test_list and test_list2 to test_list2\n");
	list_for_each_entry(iter, &test_list2, list_node) {
		printf("index of entry is %d\n", iter->index);
	}
}

struct list_tree {
	int index;
	struct list_tree *parent;
	struct list_head list;
	struct list_head child;
};

#define TREE_NODES 30
struct list_tree nodes[TREE_NODES];

void node_init(struct list_tree *node, int val)
{
	if (!node)
		return;

	node->parent = NULL;
	node->index = val;
	INIT_LIST_HEAD(&node->list);
	INIT_LIST_HEAD(&node->child);
}

typedef void *(*traverse_lt_func)(struct list_tree *me, void *data, int depth);
void *traverse_list_tree(struct list_tree *start, traverse_lt_func pre,
		void *data)
{
	struct list_tree *nd, *n_nd;
	int depth = 0;
	void *ret;

	/* We started with a tree node, iterate all children */
	for (nd = list_first_entry_or_null(&start->child, struct list_tree, list);
		nd; nd = n_nd) {

		if (pre && ((ret = pre(nd, data, depth)) != NULL))
			return ret;

		/* If we have child, go down */
		if ((n_nd = list_first_entry_or_null(&nd->child, struct list_tree, list))) {
			/* Depth first...do children */
			depth++;
			continue;
		}

		/* ok, try next sibling instead. */
		if (nd->list.next != &nd->parent->child) {
			n_nd = list_next_entry(nd, struct list_tree, list);
			continue;
		} else {
			n_nd = nd;
			/* Walk up to next valid sibling. */
			do {
				n_nd = n_nd->parent;
				depth--;
				if (n_nd == start)
					return NULL;
			} while (n_nd->list.next == &n_nd->parent->child);
			n_nd = list_next_entry(n_nd, struct list_tree, list);
		}
	}
	return NULL;
}

void *show_tree(struct list_tree *me, void *data, int depth)
{
	printf("node: %*c %d\n", depth*3, ' ', me->index);
	return NULL;
}

void list_tree_test()
{
	int i;
	
	for (i = 0; i < TREE_NODES; i++)
		node_init(&nodes[i], i);
	// for (i = 0; i < TREE_NODES; i++)
	// 	printf("node: %d\n", nodes[i].index);

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

	traverse_list_tree(&nodes[0], show_tree, NULL);
}

int main()
{
	// isempty_test();
	// islast_test();
	// isfirst_test();
	// list_add_test();
	// list_add_tail_test();
	// list_for_each_entry_test(1);
	// list_del_test();
	// list_for_each_test();
	// list_first_entry_test();
	// list_next_entry_test();
	// list_for_each_entry_reverse_test();
	list_for_each_entry_safe_test(1);
	// list_move_test();
	// list_move_tail_test();
	// list_splice_tail_test();
	// list_tree_test();

	return 0;
}
