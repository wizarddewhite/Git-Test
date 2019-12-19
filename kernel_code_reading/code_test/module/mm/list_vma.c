#include <linux/init.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
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

void list_vma(struct task_struct *task)
{
	struct mm_struct *mm = get_task_mm(task);
	struct vm_area_struct *vma;

	if (!mm) {
		printk(KERN_ERR "No mm\n");
		return;
	}
	
	down_read(&mm->mmap_sem);
	for (vma = mm->mmap ; vma ; vma = vma->vm_next) {
		printk(KERN_ERR "%08lx - %08lx\n", vma->vm_start, vma->vm_end);
	}
	up_read(&mm->mmap_sem);

	mmput(mm);
}

static int list_vma_init(void)
{
	if (!get_task())
		return -1;

	list_vma(task);

	put_task();
	return -1;
}
static void list_vma_exit(void)
{
}
module_init(list_vma_init);
module_exit(list_vma_exit);

