/*
 * =====================================================================================
 *
 *       Filename:  hello_world.c
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

static unsigned int num;
module_param(num, uint, S_IRUGO);
MODULE_PARM_DESC(num, "Number used in this example");

static int int_array[6];
int narr;
module_param_array(int_array, int, &narr, 0644);
MODULE_PARM_DESC(int_array, "A integer array");

static int hello_init(void)
{
	int i;
	printk(KERN_ALERT "Hello, world %d\n", num);

	for (i=0; i < narr; i++)
		printk(KERN_ALERT "int_array[%d] %d\n", i, int_array[i]);

	request_module("e1000e");

	return 0;
}
static void hello_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);
