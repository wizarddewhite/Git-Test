/*
 * =====================================================================================
 *
 *       Filename:  each_netdev.c
 *
 *    Description:  http://www.math.uaa.alaska.edu/~ssiewert/a320_code/EXAMPLES/Cooperstein-Drivers/s_25/lab1_devices.c
 *
 *        Version:  1.0
 *        Created:  10/11/2013 02:38:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>

static void printit(struct net_device *dev)
{
	pr_info("name = %6s irq=%4d trans_start=%12lu last_rx=%12lu\n",
		dev->name, dev->irq, dev->trans_start, dev->last_rx);
}

static int __init my_init(void)
{
	struct net_device *dev;
	pr_info("Hello: module loaded at 0x%p\n", my_init);

	/* either of these methods will work */
	for_each_netdev(&init_net, dev)
	    printit(dev);
	/*
	   dev = first_net_device(&init_net);
	   while (dev) {
	   printit(dev);
	   dev = next_net_device(dev);
	   }
	 */

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_DESCRIPTION("LDD:2.0 s_25/lab1_devices.c");
MODULE_LICENSE("GPL v2");
