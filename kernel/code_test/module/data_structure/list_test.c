#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>

MODULE_LICENSE("Dual BSD/GPL");

#define NODES	10

struct list_dummy {
	struct list_head list_node;
	int index;
};

LIST_HEAD(test_list);
struct list_dummy elem[NODES];

static int list_test_init(void)
{
	int i;
	struct list_dummy *iter;

	for (i = 0; i < NODES; i++) {
		elem[i].index = i;
		list_add_tail(&elem[i].list_node, &test_list);
	}
	list_for_each_entry(iter, &test_list, list_node)
		printk(KERN_ERR "%d\n", iter->index);
	return 0;
}

static void list_test_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(list_test_init);
module_exit(list_test_exit);
