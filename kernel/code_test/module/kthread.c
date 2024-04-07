#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
MODULE_LICENSE("Dual BSD/GPL");

struct task_struct *event_thread;

static int event_handler(void * dummy)
{
	printk("%s: running...\n", __func__);
	while(!kthread_should_stop()) {
		printk("waiting event\n");

		msleep(200);
		printk("\tprocessing event...\n");
	}
	printk("%s: quit...\n", __func__);
	return 0;
}

static int kthread_init(void)
{
	int ret = 0;

	event_thread = kthread_run(event_handler, NULL, "sema_event");
	if (IS_ERR(event_thread)) {
		ret = PTR_ERR(event_thread);
		pr_err("%s: Failed to start kthread (%d)\n",
			__func__, ret);
		return ret;
	}

	return 0;
}
static void kthread_exit(void)
{
        printk(KERN_ALERT "set thread stop state\n");
	kthread_stop(event_thread);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(kthread_init);
module_exit(kthread_exit);


