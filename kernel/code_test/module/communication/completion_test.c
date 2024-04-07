#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/timer.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

struct completion comp_node;
struct timer_list my_timer;

void my_timer_callback(unsigned long data)
{
	printk("my_timer_callback called (%ld).\n", jiffies);

	complete(&comp_node);
}

int completion_test_init( void )
{
	printk(KERN_ERR "completion test\n");

	init_completion(&comp_node);

	setup_timer(&my_timer, my_timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000) );

	printk(KERN_ERR "wait for completion\n");
	wait_for_completion(&comp_node);
	printk(KERN_ERR "done!\n");

	return 0;
}

void completion_test_exit( void )
{
	del_timer(&my_timer);
	return;
}

module_init(completion_test_init);
module_exit(completion_test_exit);
