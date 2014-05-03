/*
 * =====================================================================================
 *
 *       Filename:  hlist_case.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 02:37:26 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "hlist_case.h"

struct hlist_head head[HASHENTRIES];

#define ENTRY_NUM 60
struct dummy_struct hlist_entry[ENTRY_NUM];

/*
 * After several days, I found I forget the detail of how hlist works.
 * Here is a draft chart to illustrate my understanding.
 *
 * hlist_head
 * +--------+
 * |        | -> hlist_node -> hlist_node -> hlist_node
 * +--------+
 * |        | -> hlist_node -> hlist_node -> hlist_node
 * +--------+
 * |        | -> hlist_node -> hlist_node -> hlist_node
 * +--------+
 *
 * The concept is simple,
 * first find a 'slot' through a hash function
 * then insert/search an element in this slot.
 * And there is no order in a slot.(No sure there would be some special case.)
 *
 */

int hlist_init(struct hlist_head *hash)
{
	int i;
	int *p;

	/* Prove that INIT_HLIST_HEAD do is clean the hash array */
	/* 
	p = (int*)hash;
	for (i = 0; i< HASHENTRIES; i++)
		p[i] = i;
	*/

	for (i = 0; i< HASHENTRIES; i++)
		INIT_HLIST_HEAD(&hash[i]);

	return 0;
}

void hlist_init_test()
{
	hlist_init(head);
}

struct hlist_head *name_hash(struct hlist_head *head, struct dummy_struct *entry)
{
	int hash;

	hash = entry->index % HASHENTRIES;
	return &head[hash];
}

void hlist_add_head_test()
{
	int i;
	struct dummy_struct *dummy;

	hlist_init(head);

	/* create dummy entry */
	for (i=0; i<ENTRY_NUM; i++) {
		hlist_entry[i].index = i;
		hlist_entry[i].name_hlist.next = NULL;
		hlist_entry[i].name_hlist.pprev = NULL;
		snprintf(hlist_entry[i].name, 12, "test_dev%d", i);
	}

	/* insert to the head */
	for (i=0; i<ENTRY_NUM; i++) {
		hlist_add_head(&hlist_entry[i].name_hlist,
				name_hash(head, &hlist_entry[i]));
	}

	for (i=0; i<HASHENTRIES; i++) {
		printf("head[%d] : \n", i);
		hlist_for_each_entry(dummy, &head[i], name_hlist) {
			printf("\tindex:%2d, name: %s\n", 
					dummy->index, dummy->name);
		}
	}
}

int main()
{
	hlist_add_head_test();
	return 0;
}
