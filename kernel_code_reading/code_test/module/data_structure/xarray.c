#include <linux/init.h>
#include <linux/module.h>
#include <linux/xarray.h>

MODULE_LICENSE("Dual BSD/GPL");

static DEFINE_XARRAY(array);

void test_store_load(void)
{
	void *entry;
	xa_store(&array, 1, xa_mk_value(1), GFP_KERNEL);
	entry = xa_load(&array, 1);
	if (entry)
		printk(KERN_ERR "index 1 has value %lu\n", xa_to_value(entry));
	else
		printk(KERN_ERR "index 1 has no value\n");
	printk("xa is %sempty\n", xa_empty(&array)?"":"not ")

	xa_destroy(&array);
	printk("destroy xarray\n");
	entry = xa_load(&array, 1);
	if (entry)
		printk(KERN_ERR "index 1 has value %lu\n", xa_to_value(entry));
	else
		printk(KERN_ERR "index 1 has no value\n");
}

static int xarry_test_init(void)
{
	test_store_load();
	return 0;
}
static void xarry_test_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(xarry_test_init);
module_exit(xarry_test_exit);
