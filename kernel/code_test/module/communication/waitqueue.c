#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
MODULE_LICENSE("Dual BSD/GPL");

struct proc_dir_entry *proc;
struct task_struct *wait_thread;
wait_queue_head_t yw_wait;

static int test_proc_show(struct seq_file *m, void *v) {
	seq_printf(m, "Hello proc!\n");
	return 0;
}

static int trigger_open(struct inode *inode, struct  file *file) {
	return single_open(file, test_proc_show, NULL);
}

static ssize_t trigger_write(struct file *file, const char __user *buf,
			       size_t length, loff_t *ppos)
{
	printk("%s: place holder\n", __func__);
	wake_up_interruptible(&yw_wait);
	return length;
}
static struct file_operations trigger_ops = {
	.owner   = THIS_MODULE,
	.open    = trigger_open,
	.read    = seq_read,
	.write   = trigger_write,
	.llseek  = seq_lseek,
	.release = seq_release,
};

static int waiting_thread(void * dummy)
{
	printk("%s: running...\n", __func__);
	while(!kthread_should_stop()) {
		DEFINE_WAIT(wait);
		printk("waiting event\n");

		prepare_to_wait(&yw_wait, &wait, TASK_INTERRUPTIBLE);
		schedule();
		printk("\tprocessing event... %s\n", (char*)dummy);
		finish_wait(&yw_wait, &wait);
	}
	printk("%s: quit...\n", __func__);
	return 0;
}

static int waitqueue_init(void)
{
	int ret = -1;

	proc = proc_create("yw_wq_trigger", 0666, NULL, &trigger_ops);
	if (!proc) {
		printk(KERN_INFO "Failed to create /proc/TEST/rw\n");
		return ret;
	}
	init_waitqueue_head(&yw_wait);

	wait_thread = kthread_run(waiting_thread, "test", "wait_queue_example");
	if (IS_ERR(wait_thread)) {
		ret = PTR_ERR(wait_thread);
		pr_err("%s: Failed to start kthread (%d)\n",
			__func__, ret);
		return ret;
	}

	return 0;
}
static void waitqueue_exit(void)
{
	printk(KERN_ALERT "set thread stop state\n");
	kthread_stop(wait_thread);
	proc_remove(proc);
	printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(waitqueue_init);
module_exit(waitqueue_exit);


