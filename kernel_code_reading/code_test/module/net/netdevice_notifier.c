/*
 * =====================================================================================
 *
 *       Filename:  netdevice_notifier.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/31/2014 09:51:26 AM
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

static int my_netdev_event(struct notifier_block *this,
				 unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	dev_info(&dev->dev, "Hey, I have some news, %lu\n", event);
	
	switch (event)
	{
	case NETDEV_REGISTER:
		dev_info(&dev->dev, "I am registered\n");
		break;
	case NETDEV_UP:
		dev_info(&dev->dev, "Nice to meet you\n");
		break;
	case NETDEV_DOWN:
		dev_info(&dev->dev, "See you\n");
		break;
	}

	return NOTIFY_DONE;
}

static struct notifier_block	my_netdev_cb = {
	.notifier_call = my_netdev_event,
};

static int __init my_init(void)
{
	pr_info("Hello: module loaded at 0x%p\n", my_init);

	register_netdevice_notifier(&my_netdev_cb);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");

	unregister_netdevice_notifier(&my_netdev_cb);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL v2");
