#include <linux/init.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
MODULE_LICENSE("Dual BSD/GPL");

int pid = 1;
module_param(pid, int, 0);

struct pid *p;
struct task_struct *task;

struct task_struct *get_task(void)
{

	p = find_get_pid(pid);
	if (!p) {
		printk(KERN_ERR "No such pid: %d\n", pid);
		return NULL;
	}

	task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
	if (!task) {
		printk(KERN_ERR "No such task: %d\n", pid);
		put_pid(p);
		return NULL;
	}
	printk(KERN_ERR "task: %s\n", task->comm);
	return task;
}

void put_task(void)
{
	put_task_struct(task);
	put_pid(p);
	return;
}

static int list_vma_init(void)
{
	if (!get_task())
		return -1;

	put_task();
	return -1;
}
static void list_vma_exit(void)
{
}
module_init(list_vma_init);
module_exit(list_vma_exit);

