#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/slab.h>
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int num;
module_param(num, uint, S_IRUGO);
struct page **pages;

static int page_allocation_init(void)
{
	unsigned int i;
	cycles_t start, end;

	pages = kmalloc(num * sizeof(struct page *), GFP_KERNEL);

	if (!pages) {
		printk(KERN_ERR "not enough page structures\n");
		return -1;
	}


	start = get_cycles();
	for (i = 0; i < num; i++) {
		pages[i] = alloc_pages(GFP_KERNEL, 3);
	}

	for (i = 0; i < num; i++) {
		__free_pages(pages[i], 3);
	}
	end = get_cycles();
	pr_err("time used: %llu cycles\n", end - start);

	return 0;
}
static void page_allocation_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(page_allocation_init);
module_exit(page_allocation_exit);
