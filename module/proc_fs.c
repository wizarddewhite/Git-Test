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

MODULE_LICENSE("Dual BSD/GPL");

static struct proc_dir_entry *proc_root = NULL;

static int proc_fs_init(void)
{
	/* Create the root proc directory */
	if (!proc_root) {
		proc_root = proc_mkdir("TEST", NULL);
		if (!proc_root) {
			printk(KERN_INFO "Failed to create /proc/TEST\n");
			return -1;
		}
	}
        return 0;
}

static void proc_fs_exit(void)
{
	/* Remove the root proc directroy */
	remove_proc_entry("TEST", NULL);
}

module_init(proc_fs_init);
module_exit(proc_fs_exit);
