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
	printk("xa is %sempty\n", xa_empty(&array)?"":"not ");

	xa_destroy(&array);
	printk("destroy xarray\n");
	entry = xa_load(&array, 1);
	if (entry)
		printk(KERN_ERR "index 1 has value %lu\n", xa_to_value(entry));
	else
		printk(KERN_ERR "index 1 has no value\n");
	printk("xa is %sempty\n", xa_empty(&array)?"":"not ");
	xa_dump(&array);
}

void test_xas(void)
{
	void *entry;
	XA_STATE(xas, &array, 0);

	xa_store(&array, 1, xa_mk_value(1), GFP_KERNEL);
	xa_store(&array, 2, xa_mk_value(2), GFP_KERNEL);
 	/* xas_find() - Find the next present entry in the XArray. */
	entry = xas_find(&xas, ULONG_MAX);
	printk(KERN_ERR "the first entry has value %lu\n", xa_to_value(entry));
	entry = xas_next_entry(&xas, ULONG_MAX);
	printk(KERN_ERR "the second entry has value %lu\n", xa_to_value(entry));

	/* replace index 2 with value 5 */
	xas_store(&xas, xa_mk_value(5));

	xas_set(&xas, 0);
	xas_for_each(&xas, entry, ULONG_MAX)
		printk(KERN_ERR "entry %lu\n", xa_to_value(entry));

}

void test_xas2(void)
{
	XA_STATE(xas, &array, 0);

	/* store value in index 0 */
	xas_store(&xas, xa_mk_value(7));
	xas_set(&xas, 5);
	/* store value in index 5 */
	xas_store(&xas, xa_mk_value(17));

	xa_dump(&array);
}

void test_xas_order(void)
{
	XA_STATE(xas, &array, 0);

	// order 2
	xas_set_order(&xas, 0, 2);
	xas_store(&xas, xa_mk_value(2));
	xas_set(&xas, 4);
	xas_store(&xas, xa_mk_value(2));
	xa_dump(&array);

	// order 1
	xas_set_order(&xas, 0, 1);
	xas_store(&xas, xa_mk_value(1));
	xa_dump(&array);

	// order 3
	xas_set_order(&xas, 0, 3);
	xas_store(&xas, xa_mk_value(3));
	xa_dump(&array);
}

void test_xas_order2(void)
{
	XA_STATE(xas, &array, 0);

	// order 19
	xas_set_order(&xas, 0, 19);
	xas_store(&xas, xa_mk_value(19));
	xa_dump(&array);

	// order 18
	xas_set_order(&xas, 0, 18);
	xas_store(&xas, xa_mk_value(18));
	xa_dump(&array);
}

static int xarry_test_init(void)
{
	test_store_load();
	test_xas();
	test_xas_order();
	return 0;
}
static void xarry_test_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(xarry_test_init);
module_exit(xarry_test_exit);
