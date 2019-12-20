#include <linux/init.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
MODULE_LICENSE("Dual BSD/GPL");

int pid = 1;
module_param(pid, int, 0);
unsigned long address = 0;
module_param(address, ulong, 0);

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

void _show_pmd(struct mm_struct *mm, unsigned long address)
{
	struct page *page;
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;
	pmd_t pmde;
	pte_t *pte;
	unsigned long pfn;
	pgd = pgd_offset(mm, address);

	if (!pgd_present(*pgd))
		return;
	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		return;
	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		return;

	pmd = pmd_offset(pud, address);
	pmde = READ_ONCE(*pmd);

	if (!pmd_present(pmde)) {
		printk(KERN_ERR "pmd not present\n");
		return;
	}

	page = pmd_page(pmde);
	printk(KERN_ERR "pmd page at %lx is %s compound\n",
			page_to_pfn(page), PageCompound(page)?"":"not");

	if (pmd_trans_huge(pmde))
		return;

	pte = pte_offset_map(pmd, address);
	if (!pte_present(*pte)) {
		printk(KERN_ERR "pte not present\n");
		return;
	}

	pfn = pte_pfn(*pte);
	printk(KERN_ERR "pte page at %lx is %s compound\n",
			pfn, PageCompound(pfn_to_page(pfn))?"":"not");

}

void show_pmd(struct task_struct *task)
{
	struct mm_struct *mm = get_task_mm(task);
	struct vm_area_struct *vma;

	if (!mm) {
		printk(KERN_ERR "No mm\n");
		return;
	}

	down_read(&mm->mmap_sem);

	vma = find_vma(mm, address);
	printk(KERN_ERR "Find vma for address: %08lx [%08lx - %08lx]\n",
			address, vma->vm_start, vma->vm_end);
	printk(KERN_ERR "\tvma->vm_pgoff: %lx %s equal to vma->vm_start\n",
			vma->vm_pgoff,
			vma->vm_start == (vma->vm_pgoff << PAGE_SHIFT) ?
			"" : "not");

	_show_pmd(mm, address);

	up_read(&mm->mmap_sem);
	mmput(mm);
}

static int show_pmd_init(void)
{
	if (!get_task())
		return -1;

	show_pmd(task);

	put_task();
	return -1;
}
static void show_pmd_exit(void)
{
}
module_init(show_pmd_init);
module_exit(show_pmd_exit);

