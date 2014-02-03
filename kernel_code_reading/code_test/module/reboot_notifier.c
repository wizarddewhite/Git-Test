/*
 * =====================================================================================
 *
 *       Filename:  reboot_notifier.c
 *
 *    Description:  http://blog.csdn.net/kernel_learner/article/details/7542071
 *
 *        Version:  1.0
 *        Created:  02/03/2014 11:09:07 AM
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
#include <linux/kernel.h>
#include <linux/notifier.h>

#include <linux/reboot.h>
static int myreboot(struct notifier_block *self, unsigned long event, void *data)
{
	printk(KERN_ALERT "Just a test! Event code: %li! System reboot now...",
			event);
	return NOTIFY_OK;
}

static struct notifier_block myreboot_notifier = {
	.notifier_call = myreboot,
};

static int myreboot_init(void)
{
	register_reboot_notifier(&myreboot_notifier);
	return 0;
}

static void myreboot_exit(void)
{
	unregister_reboot_notifier(&myreboot_notifier);
}

module_init(myreboot_init);
module_exit(myreboot_exit);

MODULE_AUTHOR("dj");
MODULE_DESCRIPTION("just a notify test!");
MODULE_LICENSE("Dual BSD/GPL");
