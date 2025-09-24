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

static struct task_struct *get_task(void)
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
	printk(KERN_ERR "task: %d:%s\n", pid, task->comm);
	return task;
}

static void put_task(void)
{
	put_task_struct(task);
	put_pid(p);
	return;
}

static void _show_pmd(struct mm_struct *mm, unsigned long address)
{
	struct page *page;
	pgd_t *pgdp;
	p4d_t *p4dp;
	pud_t *pudp;
	pmd_t *pmdp;
	pmd_t pmd;
	pte_t *ptep;
	unsigned long pfn;
	pgdp = pgd_offset(mm, address);

	if (!pgd_present(*pgdp))
		return;
	p4dp = p4d_offset(pgdp, address);
	if (!p4d_present(*p4dp))
		return;
	pudp = pud_offset(p4dp, address);
	if (!pud_present(*pudp))
		return;

	pmdp = pmd_offset(pudp, address);
	pmd = READ_ONCE(*pmdp);

	if (!pmd_present(pmd)) {
		printk(KERN_ERR "pmd not present\n");
		return;
	}

	printk(KERN_ERR "pmd_index(%lx) is %lx(%lx)\n", address,
			pmd_index(address), pmd_index(address) * sizeof(pmd_t *));
	/* pmdp = pud_pgtable() + pmd_index() */
	printk(KERN_ERR "pud_pgtable %lx pmdp %lx\n",
			(unsigned long)pud_pgtable(*pudp), (unsigned long)pmdp);
	printk(KERN_ERR " => pmdp = pud_pgtable() + pmd_index()\n");
	printk(KERN_ERR "pud_pgtable()'s pfn %lx\n", page_to_pfn(virt_to_page((void *)pud_pgtable(*pudp))));
	printk(KERN_ERR "pud encodes pfn %lx\n",
			pud_val(*pudp) & pud_pfn_mask(*pudp));
	printk(KERN_ERR " => pud_pgtable(*pudp) get the vaddr and paddr is encoded\n");
	printk(KERN_ERR "pmd_pgtable_page(pmdp)'s pfn %lx\n",
			page_to_pfn(pmd_pgtable_page(pmdp)));
	printk(KERN_ERR " => pud_pgtable()'s pfn and pmd_pgtable_page(pmdp)'s pfn are the same\n");

	/* pmd_page() is different from pmd_pgtable_page() */
	page = pmd_page(pmd);
	printk(KERN_ERR "pmd_pfn(pmd) is  %lx\n", pmd_pfn(pmd));
	printk(KERN_ERR "pmd_page(pmd) is %lx is %s compound\n",
			page_to_pfn(page), PageCompound(page)?"":"not");
	printk(KERN_ERR " => pmd_page() != pmd_pgtable_page()\n");

	if (pmd_trans_huge(pmd)) {
		printk(KERN_ERR "This is a trans huge pmd\n");
		printk(KERN_ERR "\tpage_count: %d\n", page_count(page));
		printk(KERN_ERR "\tmap  count: %d\n", folio_mapcount(page_folio(page)));
		printk(KERN_ERR "\tcom mapcnt: %d\n", folio_mapcount(page_folio(page)));
		// ret = memory_failure(page_to_pfn(page), 0);
		// printk(KERN_ERR "ret %d\n", ret);
		return;
	}

	/*
	 * Need to EXPORT_SYMBOL(___pte_offset_map);
	 */
	ptep = pte_offset_map(pmdp, address);
	if (!pte_present(*ptep)) {
		printk(KERN_ERR "pte not present\n");
		return;
	}

	printk(KERN_ERR "pmd_page_vaddr %lx ptep %lx, pte_index %lx(%lx)\n",
			(unsigned long)pmd_page_vaddr(*pmdp), (unsigned long)ptep,
			pte_index(address), pte_index(address) * sizeof(pte_t *));
	printk(KERN_ERR " => ptep = pmd_page_vaddr(*pmdp) + pte_index()\n");

	printk(KERN_ERR "pmd_pfn(*pmdp) %lx\n", pmd_pfn(*pmdp));
	printk(KERN_ERR "pte_lockptr page's pfn %lx\n",
			page_to_pfn(pmd_page(pmd)));
	printk(KERN_ERR "ptep_lockptr page's pfn %lx\n",
			page_to_pfn(virt_to_page(ptep)));
	printk(KERN_ERR " => pte_lockptr/ptep_lockptr page is both pmd_pfn() page\n");

	printk(KERN_ERR "pmd_page_vaddr %lx pa %lx\n",
			(unsigned long)pmd_page_vaddr(*pmdp), __pa((unsigned long)pmd_page_vaddr(*pmdp)));

	pfn = pte_pfn(*ptep);
	page = pfn_to_page(pfn);
	printk(KERN_ERR "pte page at %lx is %s compound\n",
			pfn, PageCompound(pfn_to_page(pfn))?"":"not");
	printk(KERN_ERR "\tpage_count: %d\n", page_count(page));
	printk(KERN_ERR "\tcom mapcnt: %d\n", folio_mapcount(page_folio(page)));

	// ret = split_huge_page(page);
	// ret = memory_failure(pfn, 0);
	pte_unmap(ptep);
}

static void show_pmd(struct task_struct *task)
{
	struct mm_struct *mm = get_task_mm(task);
	struct vm_area_struct *vma;

	if (!mm) {
		printk(KERN_ERR "No mm\n");
		return;
	}

	mmap_read_lock(mm);

	vma = find_vma(mm, address);
	printk(KERN_ERR "Find vma for address: %08lx [%08lx - %08lx]\n",
			address, vma->vm_start, vma->vm_end);
	printk(KERN_ERR "\tvma->vm_pgoff: %lx %s equal to vma->vm_start\n",
			vma->vm_pgoff,
			vma->vm_start == (vma->vm_pgoff << PAGE_SHIFT) ?
			"" : "not");

	_show_pmd(mm, vma->vm_start);

	mmap_read_unlock(mm);
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

MODULE_DESCRIPTION ("ywtest");
