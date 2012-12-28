/*
 * =====================================================================================
 *
 *       Filename:  oops.c
 *
 *    Description:  This is an example of oops in kernel.
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

static int oops_example_init(void)
{
	int *p = NULL;
	*p = 1;
        return 0;
}
static void oops_example_exit(void)
{
}
module_init(oops_example_init);
module_exit(oops_example_exit);
