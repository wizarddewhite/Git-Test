/*
 * =====================================================================================
 *
 *       Filename:  char_dev.c
 *
 *    Description:  copied from http://linuxgazette.net/125/mishra.html
 *                  sudo mknod /dev/my_device c 222 0;  check /proc/device for
 *                  the major number first.
 *
 *        Version:  1.0
 *        Created:  08/08/2013 05:10:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include<linux/module.h>
#include<linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/current.h>
#include <asm/segment.h>
#include <asm/uaccess.h>


MODULE_AUTHOR("ranjmis");
MODULE_DESCRIPTION("A simple char device");
MODULE_LICENSE("Dual BSD/GPL");

static int r_init(void);
static void r_cleanup(void);

module_init(r_init);
module_exit(r_cleanup);

char my_data[80]="hi from kernel"; /* our device */

int my_open(struct inode *inode,struct file *filep)
{
	/*MOD_INC_USE_COUNT;*/ /* increments usage count of module */
	return 0;
}

int my_release(struct inode *inode,struct file *filep)
{
	/*MOD_DEC_USE_COUNT;*/ /* decrements usage count of module */
	return 0;
}
ssize_t my_read(struct file *filep,char *buff,size_t count,loff_t *offp )
{
	/* function to copy kernel space buffer to user space*/
	if ( copy_to_user(buff,my_data,strlen(my_data)) != 0 )
		printk( "Kernel -> userspace copy failed!\n" );
	return strlen(my_data);

}
ssize_t my_write(struct file *filep,const char *buff,size_t count,loff_t *offp )
{
	/* function to copy user space buffer to kernel space*/
	if ( copy_from_user(my_data,buff,count) != 0 )
		printk( "Userspace -> kernel copy failed!\n" );
	return 0;
}

struct file_operations my_fops={
	open: my_open,
	read: my_read,
	write: my_write,
	release:my_release,
};

/**
 * Module/class support
 */
static char *mydev_devnode(struct device *dev, umode_t *mode)
{
	if (mode && (MINOR(dev->devt) == 0))
		*mode = S_IRUGO | S_IWUGO;

	return kasprintf(GFP_KERNEL, "my_dev/%s", dev_name(dev));
}

struct class *my_class;
struct device *my_dev;
dev_t devt;
struct cdev my_device;
static int r_init(void)
{
	int ret;

	printk("<1>hi\n");

	/* shown in /sys/class/my_device */
	my_class = class_create(THIS_MODULE, "my_device");
	if (IS_ERR(my_class)) {
		ret = PTR_ERR(my_class);
		goto err_class;
	}

	/* create device node under /dev for this class */
	my_class->devnode = mydev_devnode;

	ret = alloc_chrdev_region(&devt, 0, MINORMASK, "my_device");
	if (ret)
		return -1;

	printk("  Major number is assigned to %d\n", MAJOR(devt));
	printk("  Minor number is assigned to %d\n", MINOR(devt));

	cdev_init(&my_device, &my_fops);
	cdev_add(&my_device, devt, 1);

	/* device shown in /dev/my_dev/my_device */
	my_dev = device_create(my_class, NULL, devt, NULL, "my_device");

	return 0;
err_class:
	return ret;
}

static void r_cleanup(void)
{
	printk("<1>bye\n");
	device_destroy(my_class, devt);
	cdev_del(&my_device);
	unregister_chrdev_region(devt, MINORMASK);
	class_destroy(my_class);
	return ;
}
