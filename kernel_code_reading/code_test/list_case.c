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

LIST_HEAD(test_list);

struct list_dummy elem[10];

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

	for (i = 1; i < 11; i++)
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

void list_for_each_entry_reverse_test()
{
	struct list_dummy *iter;

	list_for_each_entry_reverse(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}

}

void list_for_each_entry_safe_test()
{
	struct list_dummy *iter;
	struct list_dummy *tmp;

	list_for_each_entry_safe(iter, tmp, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

int main()
{
	list_add_tail_test();
	list_for_each_entry_safe_test();

	return 0;
}
