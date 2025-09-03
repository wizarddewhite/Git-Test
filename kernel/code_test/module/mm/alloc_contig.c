#include <linux/init.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/sched/mm.h>
#include <linux/mm.h>


/*
 * Need to EXPORT_SYMBOL(alloc_contig_pages_noprof);
 */
static int alloc_contig_init(void)
{
	struct page *p;

	p = alloc_contig_pages(8, GFP_KERNEL, 1, NULL);
	if (!p)
		return -ENOMEM;

	free_contig_range(page_to_pfn(p) , 8);
	return -1;
}

static void alloc_contig_exit(void)
{
}
module_init(alloc_contig_init);
module_exit(alloc_contig_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION ("ywtest");
