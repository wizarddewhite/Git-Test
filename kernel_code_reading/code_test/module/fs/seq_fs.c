/*
 * =====================================================================================
 *
 *       Filename:  seq_fs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2012 10:21:29 AM
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
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define CONTENT_SIZE 400
char buf[CONTENT_SIZE] = "1234567890";

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	printk(KERN_ALERT "my_seq_start called with pos:%lld\n", *pos);
	if (*pos >= CONTENT_SIZE)
		return NULL;

	return buf + *pos;
}

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	if (*pos >= CONTENT_SIZE)
		return NULL;

	(*pos) = (*pos) + 50;

	return buf + *pos;
}

static void my_seq_stop(struct seq_file *s, void *v)
{
	printk(KERN_ALERT "my_seq_stop called\n");
}

static int my_seq_show(struct seq_file *s, void *v)
{
	char *p = (char*)v;
	char data[51] = {0};
	int i;

	for (i=0; (i<50) && (p<buf+CONTENT_SIZE); i++)
	{
		data[i] = *(p + i);
	}

	seq_printf(s, "show: %p:%s\n", v, data);

	return 0;
}
static struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next  = my_seq_next,
	.stop  = my_seq_stop,
	.show  = my_seq_show,
};

static int seq_proc_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_ops);
}

static ssize_t seq_proc_write(struct file *filp, const char __user *buffer,
		size_t count, loff_t *ppos)
{
	if (*ppos > CONTENT_SIZE)
		return 0;
	if (*ppos + count > CONTENT_SIZE)
		count = CONTENT_SIZE - *ppos;

	if (copy_from_user(buf + *ppos, buffer, count))
		return -EFAULT;

	*ppos += count;

	return count;
}

static struct file_operations seq_proc_ops = {
	.owner   = THIS_MODULE,
	.open    = seq_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
	.write   = seq_proc_write,
};

static int seq_fs_init(void)
{
	proc_create("seq_fs", 0, NULL, &seq_proc_ops);
	return 0;
}

static void seq_fs_exit(void)
{
	remove_proc_entry("seq_fs", NULL);
}

module_init(seq_fs_init);
module_exit(seq_fs_exit);

