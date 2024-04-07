/*
 * =====================================================================================
 *
 *       Filename:  ida_idr_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2012 11:38:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

static DEFINE_IDA(test_id_ida);
static int test_id_start = 0;
int id[10];

static int ida_idr_init(void)
{
	int res, i;

	ida_pre_get(&test_id_ida, GFP_KERNEL);

	for (i = 0; i < 10; i++) {
		res = ida_get_new_above(&test_id_ida, test_id_start, &id[i]);
		if (!res)
			test_id_start = id[i] + 1;

		printk(KERN_ALERT "ID is %d\n", id[i]);
	}

	return 0;
}
static void ida_idr_exit(void)
{
	int i;

	for (i = 0; i < 10; i++)
		ida_remove(&test_id_ida, id[i]);
}

module_init(ida_idr_init);
module_exit(ida_idr_exit);
