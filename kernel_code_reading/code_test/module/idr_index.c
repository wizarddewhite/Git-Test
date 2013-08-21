/*
 * =====================================================================================
 *
 *       Filename:  idr_index.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/21/2013 03:30:34 PM
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
#include <linux/idr.h>
MODULE_LICENSE("Dual BSD/GPL");

struct idr idr_index;
struct idr dummy;

#define MAX_INDEX 1024

static int idr_index_init(void)
{
	int index;

        printk(KERN_ALERT "idr_index test example \n");
	idr_init(&idr_index);

	index = idr_alloc(&idr_index, &dummy, 1, MAX_INDEX + 1, GFP_KERNEL);
        printk(KERN_ALERT "index get %d \n", index);

	index = idr_alloc(&idr_index, &dummy, 1, MAX_INDEX + 1, GFP_KERNEL);
        printk(KERN_ALERT "index get %d \n", index);

	index = idr_alloc(&idr_index, &dummy, 10, MAX_INDEX + 1, GFP_KERNEL);
        printk(KERN_ALERT "index get %d \n", index);

	idr_remove(&idr_index, index);
        printk(KERN_ALERT "index %d removed\n", index);

	index = idr_alloc(&idr_index, &dummy, 10, MAX_INDEX + 1, GFP_KERNEL);
        printk(KERN_ALERT "index get %d \n", index);

	index = idr_alloc(&idr_index, &dummy, 10, MAX_INDEX + 1, GFP_KERNEL);
        printk(KERN_ALERT "index get %d \n", index);

        return 0;
}
static void idr_index_exit(void)
{
	idr_destroy(&idr_index);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(idr_index_init);
module_exit(idr_index_exit);
