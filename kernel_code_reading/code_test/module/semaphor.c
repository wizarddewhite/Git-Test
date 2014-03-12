/*
 * =====================================================================================
 *
 *       Filename:  semaphor.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/11/2014 04:32:44 PM
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
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
MODULE_LICENSE("Dual BSD/GPL");

static struct semaphore eventlist_sem;
struct task_struct *event_thread;

static int event_handler(void * dummy)
{
	printk("%s: running...\n", __func__);
	while(!kthread_should_stop()) {
		printk("waiting event\n");
		if (down_timeout(&eventlist_sem, 10*HZ) == -ETIME)
			continue;

		printk("\tprocessing event...\n");
	}
	return 0;
}

static int semaphor_init(void)
{
	int ret = 0;

	sema_init(&eventlist_sem, 0);

	event_thread = kthread_run(event_handler, NULL, "sema_event");
	if (IS_ERR(event_thread)) {
		ret = PTR_ERR(event_thread);
		pr_err("%s: Failed to start EEH daemon (%d)\n",
			__func__, ret);
		return ret;
	}

	msleep(2000);
	up(&eventlist_sem);

	return 0;
}
static void semaphor_exit(void)
{
        printk(KERN_ALERT "set thread stop state\n");
	kthread_stop(event_thread);
        printk(KERN_ALERT "up the semaphor\n");
	up(&eventlist_sem);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(semaphor_init);
module_exit(semaphor_exit);


