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

void listadd_test()
{
	int i;

	INIT_LIST_HEAD(&elem[0].list_node);

	for (i = 0; i < 10; i++)
	{
		elem[i].index = i;
		list_add(&elem[i].list_node, &test_list);
	}
}

void listaddtail_test()
{
	int i;

	INIT_LIST_HEAD(&elem[0].list_node);

	for (i = 0; i < 10; i++)
	{
		elem[i].index = i;
		list_add_tail(&elem[i].list_node, &test_list);
	}
}

void listforeach_test()
{
	struct list_head *iter;
	int i = 0;

	list_for_each(iter, &test_list)
		printf("%d\n", i++);
}

void listforeachentry_test()
{
	struct list_dummy *iter;
	int i = 0;

	list_for_each_entry(iter, &test_list, list_node)
	{
		printf("index of entry is %d\n", iter->index);
	}
}

int main()
{
	listaddtail_test();
	listforeachentry_test();

	return 0;
}
