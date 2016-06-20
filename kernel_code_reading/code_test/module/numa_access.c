#include <linux/init.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");

#define IDX 1
#define TEST_ARRAY_SIZE (1024 * IDX)

struct page *local_page, *remote_page;
char *local_mem, *remote_mem;

static int numa_access_init(void)
{
	int cpu, nid;
	int i, j;
	volatile char tmp;
	struct timespec ts;
	s64 timestamp;

	cpu = get_cpu();
	printk(KERN_ERR "current cpu: %d\n", cpu);

	local_mem = remote_mem = NULL;

	local_page = alloc_pages_node(cpu_to_mem(cpu),
			GFP_KERNEL, get_order(TEST_ARRAY_SIZE));

	if (!local_page)
		goto finish;
	local_mem = page_address(local_page);
	printk(KERN_ERR "local mem: %llx \n", page_to_phys(local_page));

	for_each_online_node(nid)
		if (nid != cpu_to_mem(cpu)) {
			remote_page = alloc_pages_node(nid,
					GFP_KERNEL, get_order(TEST_ARRAY_SIZE));
			break;
		}

	if (!remote_page)
		goto finish;
	remote_mem = page_address(remote_page);

	printk(KERN_ERR "remote mem: %llx\n", page_to_phys(remote_page));

	for (i = 0; i < 5; i++) {
		getnstimeofday(&ts);
		for (j = 0; j < TEST_ARRAY_SIZE; j++)
			tmp = local_mem[i];
		getnstimeofday(&ts);
	}

	for (i = 0; i < 5; i++) {
		getnstimeofday(&ts);
		for (j = 0; j < TEST_ARRAY_SIZE; j++)
			tmp = remote_mem[i];
		getnstimeofday(&ts);
	}

finish:
	free_pages((unsigned long)local_mem, get_order(TEST_ARRAY_SIZE));
	free_pages((unsigned long)remote_mem, get_order(TEST_ARRAY_SIZE));

	put_cpu();
	return -1;
}
static void numa_access_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(numa_access_init);
module_exit(numa_access_exit);
