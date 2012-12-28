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
	proc = create_proc_entry("rw", 0666, proc_root);
	if (!proc) {
		printk(KERN_INFO "Failed to create /proc/TEST/rw\n");
		goto fail;
	}

	/* Assign operation functions and extra parameter */
	proc->read_proc = r_proc;
	proc->write_proc = w_proc;
	proc->data = NULL;

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
