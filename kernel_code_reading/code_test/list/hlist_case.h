/*
 * =====================================================================================
 *
 *       Filename:  hlist_case.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 02:36:36 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef _H_LIST_CASE_
#define _H_LIST_CASE_

#include "hlist.h"

#define HASHENTRIES 32

struct dummy_struct {
	int index;
	char name[12];
	struct hlist_node name_hlist;
};

#endif
