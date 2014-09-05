/*
 * =====================================================================================
 *
 *       Filename:  scull_sys_fs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/17/2012 08:48:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "scull.h"

static ssize_t scull_sys_fs_show(struct kobject *kobj,
					struct kobj_attribute *attr,
					char *buf)
{
	return sprintf(buf, "%d\n", 1);
}

static struct kobj_attribute scull_sys_attr = __ATTR(scull_sys_fs,
                                   0770, scull_sys_fs_show, NULL);

static ssize_t scull_attr_show(struct kobject *kobj,
				struct attribute *attr,
				char *page)
{
	return sprintf(page, "%d\n", 1);
}

static ssize_t scull_attr_store(struct kobject *kobj, struct attribute *attr,
		const char *page, size_t length)
{
	return length;
}

static struct sysfs_ops scull_sysfs_ops = {
	.show  = scull_attr_show,
	.store = scull_attr_store,
};

static void scull_release(struct kobject *kobj)
{
	return;
}

static struct kobj_type scull_ktype = {
	.sysfs_ops = &scull_sysfs_ops,
	.release   = scull_release,
};

void scull_setup_sysfs(struct scull_dev *dev, int index)
{
	/* Test on the sysfs */
	dev->cdev.kobj.ktype = &scull_ktype;

	kobject_add(&dev->cdev.kobj, NULL, "scull%d", index);

	sysfs_create_file(&dev->cdev.kobj, &scull_sys_attr.attr);
	return;
}

void scull_cleanup_sysfs(struct scull_dev *dev, int index)
{
	/* Remove from the sysfs */
	sysfs_remove_file(&dev->cdev.kobj, &scull_sys_attr.attr);

	kobject_del(&dev->cdev.kobj);
	return;
}
