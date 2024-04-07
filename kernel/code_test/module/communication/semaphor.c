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
struct task_struct *trigger_thread;

static int event_handler(void * dummy)
{
	printk("%s: running...\n", __func__);
	while(!kthread_should_stop()) {
		printk("waiting event\n");
		if (down_timeout(&eventlist_sem, 10*HZ) == -ETIME)
			continue;

		printk("\tprocessing event...\n");
		msleep(200);
	}
	printk("%s: quit...\n", __func__);
	return 0;
}

static int event_trigger(void * dummy)
{
	printk("%s: running...\n", __func__);
	while(!kthread_should_stop()) {
		msleep(20000);
		printk("trigger event\n");
		up(&eventlist_sem);
	}
	printk("%s: quit...\n", __func__);
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

	trigger_thread = kthread_run(event_trigger, NULL, "sema_trigger");
	if (IS_ERR(trigger_thread)) {
		ret = PTR_ERR(trigger_thread);
		pr_err("%s: Failed to start trigger daemon (%d)\n",
			__func__, ret);
		kthread_stop(event_thread);
		return ret;
	}

	return 0;
}
static void semaphor_exit(void)
{
        printk(KERN_ALERT "set thread stop state\n");
	kthread_stop(event_thread);
	kthread_stop(trigger_thread);
        printk(KERN_ALERT "up the semaphor\n");
	up(&eventlist_sem);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(semaphor_init);
module_exit(semaphor_exit);


