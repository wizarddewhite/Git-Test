/*
 * Copied from 
 *        http://www.ibm.com/developerworks/linux/library/l-timers-list/index.html
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static struct timer_list my_timer;

void my_timer_callback( unsigned long data )
{
	static int times = 0;

	printk( "my_timer_callback called (%ld:%d).\n", 
			jiffies, times++);

	if (times <= 5)
		mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));

}

int timer_init( void )
{
	int ret;
	
	printk("Timer module installing\n");

	// my_timer.function, my_timer.data
	setup_timer( &my_timer, my_timer_callback, 0);

	printk( "Starting timer to fire in 200ms (%ld)\n", jiffies );
	ret = mod_timer( &my_timer, jiffies + msecs_to_jiffies(200) );
	if (ret) printk("Error in mod_timer\n");

	return 0;
}

void timer_exit( void )
{
	int ret;

	ret = del_timer( &my_timer );
	if (ret) printk("The timer is still in use...\n");

	printk("Timer module uninstalling\n");

	return;
}
module_init(timer_init);
module_exit(timer_exit);
