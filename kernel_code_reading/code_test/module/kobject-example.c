/*
 * =====================================================================================
 *
 *       Filename:  kobject-example.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/18/2012 04:25:40 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
MODULE_LICENSE("Dual BSD/GPL");

/*
 * This module shows how to create a simple subdirectory in sysfs called
 * /sys/kernel/kobject-example  In that directory, 3 files are created:
 * "foo", "baz", and "bar".  If an integer is written to these files, it can be
 * later read out of it.
 */

static int foo;
static int baz;
static int bar;

/*
 * The "foo" file where a static variable is read from and written to.
 */
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	sscanf(buf, "%du", &foo);
	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, foo_show, foo_store);

/*
 * More complex function where we determine which variable is being accessed by
 * looking at the attribute for the "baz" and "bar" files.
 */
static ssize_t b_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int var;

	if (strcmp(attr->attr.name, "baz") == 0)
		var = baz;
	else
		var = bar;
	return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	int var;

	sscanf(buf, "%du", &var);
	if (strcmp(attr->attr.name, "baz") == 0)
		baz = var;
	else
		bar = var;
	return count;
}

static struct kobj_attribute baz_attribute =
	__ATTR(baz, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, b_show, b_store);
static struct kobj_attribute bar_attribute =
	__ATTR(bar, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, b_show, b_store);


/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = {
	&foo_attribute.attr,
	&baz_attribute.attr,
	&bar_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

/*
 * The "ltc" file where a static variable is read from and written to.
 */
static int ltc;
static ssize_t ltc_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", ltc);
}

static ssize_t ltc_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	sscanf(buf, "%du", &ltc);
	return count;
}

static struct kobj_attribute ltc_attribute =
	__ATTR(ltc, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, ltc_show, ltc_store);

static int ltc1;
static ssize_t ltc1_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	return sprintf(buf, "%d\n", ltc1);
}

static ssize_t ltc1_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	sscanf(buf, "%du", &ltc1);
	return count;
}

static struct kobj_attribute ltc1_attribute =
	__ATTR(ltc1, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP, ltc1_show, ltc1_store);

static struct kobject *example_kobj;
static struct kobject *example_link_kobj;

static int __init example_init(void)
{
	int retval;

	/*
	 * Create a simple kobject with the name of "kobject_example",
	 * located under /sys/kernel/
	 *
	 * As this is a simple directory, no uevent will be sent to
	 * userspace.  That is why this function should not be used for
	 * any type of dynamic kobjects, where the name and number are
	 * not known ahead of time.
	 */
	example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
	printk("the ktype of example_kobj is %p\n", example_kobj->ktype);
	example_link_kobj = kobject_create_and_add("kobject_link_example", kernel_kobj);
	if (!example_kobj || !example_link_kobj)
		return -ENOMEM;

	kobject_set_name(example_kobj, "kobject_example_%d", 1);
	printk("the name of example_kobj is %s\n", kobject_name(example_kobj));

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	retval = sysfs_add_file_to_group(example_kobj, &ltc_attribute.attr, NULL);
	if (retval)
		kobject_put(example_kobj);

	retval = sysfs_create_file(example_kobj, &ltc1_attribute.attr);
	if (retval)
		kobject_put(example_kobj);

	retval = sysfs_create_link(example_link_kobj, example_kobj, "link_to");
	if (retval)
		printk("failed to create link to example_kobj\n");

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(example_kobj);
	kobject_put(example_link_kobj);
}

module_init(example_init);
module_exit(example_exit);
