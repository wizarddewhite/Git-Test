#include <linux/init.h>
#include <linux/module.h>
#include <linux/klist.h>

MODULE_LICENSE("Dual BSD/GPL");

#define NODES	10

struct klist test_klist;

struct klist_test_node {
	struct klist_node test_node;
	int dummy_value;
};

struct klist_test_node test_knodes[NODES];

static void klist_test_get(struct klist_node *n)
{
	struct klist_test_node *node = container_of(n, struct klist_test_node,
			test_node);
	printk(KERN_ERR "%s called on %d\n", __func__, node->dummy_value);
}

static void klist_test_put(struct klist_node *n)
{
	struct klist_test_node *node = container_of(n, struct klist_test_node,
			test_node);
	printk(KERN_ERR "%s called on %d\n", __func__, node->dummy_value);
}

static struct klist_test_node *next_test_node(struct klist_iter *i)
{
	struct klist_node *n = klist_next(i);
	struct klist_test_node *node = NULL;

	if (n)
		node = container_of(n, struct klist_test_node, test_node);

	return node;
}

void dump_klist(struct klist *list)
{
	struct klist_iter iter;
	struct klist_test_node *node;

	klist_iter_init_node(list, &iter, NULL);
	while ((node = next_test_node(&iter)))
		printk(KERN_ERR "node[%d] found\n", node->dummy_value);
	klist_iter_exit(&iter);
}

void init(void)
{
	int i;

	klist_init(&test_klist, klist_test_get, klist_test_put);

	for (i = 0; i < NODES; i ++) {
		test_knodes[i].dummy_value = i;
		/* klist_test_get is called on the node */
		klist_add_tail(&test_knodes[i].test_node, &test_klist);
	}

	printk(KERN_ERR "--- The original list ---\n");
	dump_klist(&test_klist);
}

void del_node_test(struct klist *list)
{
	struct klist_iter iter;
	struct klist_test_node *node;

	klist_iter_init_node(list, &iter, NULL);
	while ((node = next_test_node(&iter)))
		if (node->dummy_value == 5)
			break;
	klist_iter_exit(&iter);

	/* klist_test_put is called on the node */
	klist_del(&node->test_node);

	printk(KERN_ERR "--- After remove node 5 ---\n");
	dump_klist(&test_klist);
}

static int klist_test_init(void)
{
	init();
	del_node_test(&test_klist);

	return 0;
}

static void klist_test_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(klist_test_init);
module_exit(klist_test_exit);
