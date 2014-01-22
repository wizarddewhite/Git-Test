/*
 * =====================================================================================
 *
 *       Filename:  proc_fs.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/02/2012 10:36:10 AM
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
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

static struct proc_dir_entry *proc_root = NULL;

#define PROCFS_MAX_SIZE 1024
static char procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static int r_proc(char *page, char **start, off_t off, 
		int count, int *eof, void *data)
{
	int ret;

	if (off > 0) {
		/* We have finished reading. */
		ret = 0;
	} else {
		/* fill the buffer and return the size */
		memcpy(page, procfs_buffer, procfs_buffer_size);
		ret = procfs_buffer_size;
	}

	return ret;
}

static int w_proc(struct file *file, const char __user *buffer,
		unsigned long count, void *data)
{

	/* Get buffer size */
	procfs_buffer_size = count;

	if (procfs_buffer_size > PROCFS_MAX_SIZE)
		procfs_buffer_size = PROCFS_MAX_SIZE;

	/* Write data to buffer */
	if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
		return -EFAULT;

	return procfs_buffer_size;
}

static int test_proc_show(struct seq_file *m, void *v) {
	seq_printf(m, "Hello proc!\n");
	return 0;
}

static int test_proc_open(struct inode *inode, struct  file *file) {
	return single_open(file, test_proc_show, NULL);
}

static struct file_operations proc_ops = {
	.owner   = THIS_MODULE,
	.open    = test_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
};

static char in_data[20] = "This is a test\n";

static ssize_t file_write(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos)
{

	/* Get buffer size */
	if (*pos + count > 20)
		count = 20 - *pos;

	printk(KERN_ERR "%s: pos is %lld\n", __func__, *pos);
	printk(KERN_ERR "%s: count is %ld\n", __func__, count);

	/* Write data to buffer */
	if (copy_from_user(in_data + *pos, buffer, count))
		return -EFAULT;

	return count;
}

static void * file_start(struct seq_file *s, loff_t *pos)
{
	loff_t n = *pos;
	printk(KERN_ERR "%s: pos is %lld\n", __func__, *pos);

	if (n < 20)
		return in_data + n;
	else
		return NULL;
}

static void * file_next(struct seq_file *s, void *v, loff_t *pos)
{
	char *tmp = v;
	loff_t n = *pos;
	printk(KERN_ERR "%s: pos is %lld\n", __func__, n);

	n++;
	if (n < 20) {
		++*pos;
		return tmp + 1;
	}
	else
		return NULL;
}

static void file_stop(struct seq_file *s, void *v)
{
	seq_printf(s, "\n");
}

static int file_show(struct seq_file *s, void *v)
{
	char *tmp = v;

	seq_printf(s, "%c", *tmp);

	return 0;
}

static const struct seq_operations file_seq_ops = {
	.start = file_start,
	.next  = file_next,
	.stop  = file_stop,
	.show  = file_show
};

static int file_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &file_seq_ops);
}

static const struct file_operations file_ops = {
	.open    = file_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release,
	.write   = file_write,
};

static int proc_fs_init(void)
{
	struct proc_dir_entry *proc;

	/* Create the root proc directory */
	if (!proc_root) {
		proc_root = proc_mkdir("TEST", NULL);
		if (!proc_root) {
			printk(KERN_INFO "Failed to create /proc/TEST\n");
			return -1;
		}
	}

	/* Create an entry under root directory */
	proc = proc_create("r_file", 0666, proc_root, &proc_ops);
	if (!proc) {
		printk(KERN_INFO "Failed to create /proc/TEST/rw\n");
		goto fail;
	}

	proc = proc_create("rw_file", 0666, proc_root, &file_ops);
	if (!proc) {
		printk(KERN_INFO "Failed to create /proc/TEST/rw\n");
		goto fail;
	}

	/* Assign operation functions and extra parameter */
	//proc->read_proc = r_proc;
	//proc->write_proc = w_proc;
	//proc->data = NULL;

        return 0;

fail:
	remove_proc_entry("TEST", NULL);
	return -1;
}

static void proc_fs_exit(void)
{
	/* Remove the TEST/rw */
	remove_proc_entry("rw", proc_root);

	/* Remove the root proc directroy */
	remove_proc_entry("TEST", NULL);
}

module_init(proc_fs_init);
module_exit(proc_fs_exit);
