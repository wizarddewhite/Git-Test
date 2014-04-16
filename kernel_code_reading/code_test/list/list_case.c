/*
 * =====================================================================================
 *
 *       Filename:  list_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/15/2012 10:21:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "list_case.h"

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
	if (list_is_last(&test_list, &test_list))
		printf("list is last\n");
	else
		printf("list is non last\n");
}

void isfirst_test()
{
	struct list_dummy *elem;

	list_add_tail_test();

	elem = list_first_entry(&test_list, struct list_dummy, list_node);
	printf("index of first node is %d\n", elem->index);

	if (list_is_first(&elem->list_node, &test_list))
		printf("list is first\n");
	else
		printf("list is non first\n");
}

void list_add_test()
{
	int i;

	INIT_LIST_HEAD(&elem[0].list_node);

	for (i = 0; i < 10; i++)
	{
		elem[i].index = i;
		list_add(&elem[i].list_node, &test_list);
	}
}

void list_del_test()
{
	printf("prepare the list for test: \n");
	list_add_tail_test();
	list_for_each_entry_safe_test();

	list_del(&elem[7].list_node);
	printf("after delete the elment 7: \n");
	list_for_each_entry_safe_test();
}

void list_add_tail_test()
{
	int i;

	INIT_LIST_HEAD(&elem[0].list_node);

	for (i = 1; i < FIRST_LENGTH; i++)
	{
		elem[i].index = i;
		list_add_tail(&elem[i].list_node, &test_list);
	}
}

void list_for_each_test()
{
	struct list_head *iter;
	int i = 0;

	list_for_each(iter, &test_list)
		printf("%d\n", i++);
}

void list_for_each_entry_test()
{
	struct list_dummy *iter;

	list_add_tail_test();

	list_for_each_entry(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
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
	struct list_dummy *iter;

	list_add_tail_test();

	iter = list_first_entry(&test_list, struct list_dummy, list_node);
	printf("index of first node is %d\n", iter->index);


	while(1) {

		iter = list_next_entry(iter, struct list_dummy, list_node);
		printf("index of third node is %d\n", iter->index);

		if (iter->list_node.next == &test_list) {
			printf("The end of the list, no more entry\n");
			break;
		}
		else {
			iter = list_next_entry(iter, struct list_dummy, list_node);
			printf("index of forth node is %d\n", iter->index);
		}
	}

}

void list_for_each_entry_reverse_test()
{
	struct list_dummy *iter;

	list_add_tail_test();

	list_for_each_entry_reverse(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}

}

void list_for_each_entry_safe_test()
{
	struct list_dummy *iter;
	struct list_dummy *tmp;

	list_add_tail_test();

	list_for_each_entry_safe(iter, tmp, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

void list_move_test()
{
	struct list_dummy *iter, *iter2;
	struct list_dummy *tmp;

	list_for_each_entry_test();

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

void list_splice_tail_test()
{
	int i;
	struct list_dummy *iter;

	INIT_LIST_HEAD(&elem2[0].list_node);

	for (i = 0; i < SECOND_LENGTH; i++)
	{
		elem2[i].index = i + 100;
		list_add_tail(&elem2[i].list_node, &test_list2);
	}

	printf("--- create test_list2\n");
	list_for_each_entry(iter, &test_list2, list_node) {
		printf("index of entry is %d\n", iter->index);
	}

	printf("--- create test_list\n");
	list_add_tail_test();

	list_for_each_entry(iter, &test_list, list_node) {
		printf("index of entry is %d\n", iter->index);
	}

	list_splice_tail(&test_list, &test_list2);

	printf("--- merge test_list and test_list2 to test_list2\n");
	list_for_each_entry(iter, &test_list2, list_node) {
		printf("index of entry is %d\n", iter->index);
	}
}

int main()
{
	list_move_test();

	return 0;
}
