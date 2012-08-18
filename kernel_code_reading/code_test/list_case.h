/*
 * =====================================================================================
 *
 *       Filename:  list_case.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/16/2012 07:49:30 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _LIST_CASE_
#define _LIST_CASE_

#include "list.h"

struct list_dummy {
	struct list_head list_node;
	int index;
};

typedef struct list_dummy list_dummy;

void list_del_test();

void list_add_tail_test();

void list_for_each_entry_safe_test();

#endif //_LIST_CASE_
