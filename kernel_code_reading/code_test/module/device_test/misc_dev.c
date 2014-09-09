/*
 * =====================================================================================
 *
 *       Filename:  misc_dev.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/21/2012 11:38:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

#define MY_MISC_MINOR		180

char text[1024] = "my first misc dev";

static int my_misc_fops_open(struct inode *inode, struct file *filep)
{
	filep->private_data = text;
	return 0;
}

static int my_misc_fops_release(struct inode *inode, struct file *filep)
{
	filep->private_data = NULL;
	return 0;
}

static ssize_t my_misc_fops_read(struct file *filep, char __user *buf,
			      size_t count, loff_t *ppos)
{
	int offset = *(int*)ppos;

	if (offset >= 1024)
		return 0;
	else if (offset + count > 1024)
		count = 1024 - offset;
	copy_to_user(buf, filep->private_data + offset, count);
	return count;
}

static ssize_t my_misc_fops_write(struct file *filep, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	return count;
}

static const struct file_operations my_misc_fops = {
	.owner		= THIS_MODULE,
	.open		= my_misc_fops_open,
	.release	= my_misc_fops_release,
	.read		= my_misc_fops_read,
	.write		= my_misc_fops_write,
};

static struct miscdevice my_misc = {
	.minor = MY_MISC_MINOR,
	.name = "misc_dev",
	.fops = &my_misc_fops,
	.nodename = "my_misc/misc",
	.mode = S_IRUGO | S_IWUGO,
};

static int misc_dev_init(void)
{
	printk(KERN_ALERT "Hello, world %d\n", 1);
	misc_register(&my_misc);

	return 0;
}
static void misc_dev_exit(void)
{
	misc_deregister(&my_misc);
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(misc_dev_init);
module_exit(misc_dev_exit);
