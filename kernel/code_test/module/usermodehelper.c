/*
 * =====================================================================================
 *
 *       Filename:  usermodehelper.c
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
#include <linux/module.h>
#include <linux/sched.h>
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int num;
module_param(num, uint, S_IRUGO);
MODULE_PARM_DESC(num, "Number used in this example");

static int test_usermodehelper_init(void)
{
	int result = 0;
	char cmdPath[] = "/usr/bin/touch";
	char* cmdArgv[] = {cmdPath, "/touchX.txt", NULL};
	char* cmdEnvp[] = {"HOME=/", "PATH=/sbin/:/bin:/usr/bin", NULL};

	result = call_usermodehelper(cmdPath, cmdArgv, cmdEnvp, UMH_WAIT_PROC);
	printk(KERN_DEBUG "result of call_usermodehelper is %d\n",result);
	printk(KERN_DEBUG"The process is \"%s\",pid is %d\n",
			current->comm,current->pid);
        return 0;
}
static void test_usermodehelper_exit(void)
{
	int result = 0;
	char cmdPath[] = "/bin/rm";
	char* cmdArgv[] = {cmdPath, "/touchX.txt", NULL};
	char* cmdEnvp[] = {"HOME=/", "PATH=/sbin/:/bin:/usr/bin", NULL};

	result = call_usermodehelper(cmdPath, cmdArgv, cmdEnvp, UMH_WAIT_PROC);
	printk(KERN_DEBUG "result of call_usermodehelper is %d\n",result);
	printk(KERN_DEBUG"The process is \"%s\",pid is %d\n",
			current->comm,current->pid);
}
module_init(test_usermodehelper_init);
module_exit(test_usermodehelper_exit);
