/*
 * =====================================================================================
 *
 *       Filename:  rb_tree.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/15/2014 05:37:11 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#ifndef __RB_TREE__
#define __RB_TREE__

#include <linux/rbtree.h>

enum child_dir {
	left_child,
	right_child,
	root_node
};

struct dummy_struct {
	int idx;
	struct rb_node rb;
	u32 subtree_nodes;
};

#define rb_to_dummy(X) rb_entry((X), struct dummy_struct, rb)

#endif //__RB_TREE__
