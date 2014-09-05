/*
 * =====================================================================================
 *
 *       Filename:  radix_tree.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/14/2014 02:09:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/radix-tree.h>

MODULE_LICENSE("Dual BSD/GPL");

struct rdx_node {
	int id;
};

struct rdx_node nodes[10];

struct radix_tree_root my_tree;
static int radix_tree_test_init(void)
{
	int i;
	struct rdx_node *s_node;
	INIT_RADIX_TREE(&my_tree, GFP_KERNEL);

	for (i = 0; i < 10; i++)
		nodes[i].id = i;
	for (i = 0; i < 10; i++)
		radix_tree_insert(&my_tree, nodes[i].id, nodes + i);

	s_node = radix_tree_lookup(&my_tree, 4);

	printk("id 4 node is: %p:%d\n", s_node, s_node->id);
	return 0;
}
static void radix_tree_test_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(radix_tree_test_init);
module_exit(radix_tree_test_exit);
