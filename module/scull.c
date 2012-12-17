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
#include <asm/uaccess.h>

#include "scull.h"

MODULE_LICENSE("Dual BSD/GPL");

int scull_major = 0;
int scull_minor = 0;
int scull_nr_devs = 5;

struct scull_dev *scull_devices;	/* allocated in scull_init_module */

int scull_open(struct inode *inode, struct file *filp)
{
	struct scull_dev *dev; /* device information */

	dev = container_of(inode->i_cdev, struct scull_dev, cdev);
	filp->private_data = dev; /* for other methods */

	return 0;          /* success */
}

int scull_release(struct inode *inode, struct file *filp)
{
	return 0;
}

ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data; 
	ssize_t retval = 0;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	if (*f_pos >= dev->size)
		goto out;

	if (count > dev->size - *f_pos)
		count = dev->size - *f_pos;

	if (copy_to_user(buf, dev->data, count)) {
		retval = -EFAULT;
		goto out;
	}

	*f_pos += count;
	retval = count;

out:
	up(&dev->sem);
	return retval;
}

ssize_t scull_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	struct scull_dev *dev = filp->private_data;
	ssize_t retval = -ENOMEM;

	if (down_interruptible(&dev->sem)) // before enter critical section
		return -ERESTARTSYS;

	if (*f_pos >= SCULL_SIZE)
		goto out;

	if (count > SCULL_SIZE - dev->size)
		count = SCULL_SIZE - dev->size;

	if (copy_from_user(dev->data, buf, count)) {
		retval = -EFAULT;
		goto out;
	}

	*f_pos += count;
	retval = count;

	if (dev->size < *f_pos)
		dev->size = *f_pos;
out:
	up(&dev->sem);
	return retval;
}

long scull_ioctl( struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

	return 0;
}

loff_t scull_llseek(struct file *filp, loff_t off, int whence)
{
	return 0;
}

struct file_operations scull_fops = {
	.owner =    THIS_MODULE,
	.open =     scull_open,
	.release =  scull_release,
	.read =     scull_read,
	.write =    scull_write,
	.unlocked_ioctl =    scull_ioctl,
	.llseek =   scull_llseek, // lseek function for this device file
};

static void scull_setup_cdev(struct scull_dev *dev, int index)
{
	int err;
	dev_t devno = MKDEV(scull_major, scull_minor + index);

	/* Allocate memory for the virtual device */
	dev->data = kzalloc(SCULL_SIZE, GFP_KERNEL);

	/* Initialize the cdev and add to system */
	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, devno, 1);

	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static void scull_cleanup_cdev(struct scull_dev *dev, int index)
{
	if (!dev)
		return;

	/* Unregister it from the system */
	cdev_del(&dev->cdev);

	/* Release the memory for the virtual device */
	kfree(dev->data);

	return;
}

static void scull_exit_module(void)
{
	int i;
	dev_t devno = MKDEV(scull_major, scull_minor);

	/* Release the memory allocated for scull_devices */
	if (scull_devices) {
		for (i = 0; i < scull_nr_devs; i++) {
			/* Release the cdev */
			scull_cleanup_cdev(&scull_devices[i], i);
		}
		kfree(scull_devices);
	}

	/* Release the device number */
	unregister_chrdev_region(devno, scull_nr_devs);
}

static int scull_init_module(void)
{
	int result, i;
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

	/* Allocating the memory for scull_devices */
	scull_devices = kzalloc(scull_nr_devs * sizeof(struct scull_dev), GFP_KERNEL);
	if (!scull_devices) {
		result = -ENOMEM;
		goto fail;  /* Make this more graceful */
	}

        /* Initialize each device. */
	for (i = 0; i < scull_nr_devs; i++) {
		sema_init(&scull_devices[i].sem, 1); // must initialize before use
		scull_setup_cdev(&scull_devices[i], i);
	}

        return 0;

fail:
	scull_exit_module();
	return result;
}

module_init(scull_init_module);
module_exit(scull_exit_module);
