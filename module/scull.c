/*
 * =====================================================================================
 *
 *       Filename:  scull.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/13/2012 10:48:55 PM
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
#include <linux/cdev.h>
#include <linux/fs.h>
MODULE_LICENSE("Dual BSD/GPL");

int scull_major = 0;
int scull_minor = 0;
int scull_nr_devs = 5;

static int scull_init_module(void)
{
	int result;
	dev_t dev = 0;

	printk(KERN_ALERT "scull: scull_init_module\n");

	result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
	scull_major = MAJOR(dev);

	if (result < 0) {
		printk(KERN_ALERT "scull: can't get major %d\n", scull_major);
		return result;
	}
	else {
		printk(KERN_ALERT "scull: get major number %d\n", scull_major);
	}

        return 0;
}
static void scull_exit_module(void)
{
	dev_t devno = MKDEV(scull_major, scull_minor);

	unregister_chrdev_region(devno, scull_nr_devs);
}

module_init(scull_init_module);
module_exit(scull_exit_module);
