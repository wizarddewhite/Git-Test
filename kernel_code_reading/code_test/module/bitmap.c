
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/pci.h>
MODULE_LICENSE("Dual BSD/GPL");

DECLARE_BITMAP(bit_test, 50);

static int bitmap_init(void)
{
	unsigned long idx;

	pr_info("--- A test driver for bitmap ---\n");
	set_bit(40, bit_test);
	idx = find_first_bit(bit_test, 50);
	pr_info("--- first set bit %lu ---\n", idx);

	bitmap_find_next_zero_area(bit_test, 50, idx, 10, 0);
	pr_info("--- next zero area witn 10 %lu ---\n", 
		bitmap_find_next_zero_area(bit_test, 50, idx, 10, 0));

	pr_info("--- next zero area witn 9 %lu ---\n", 
		bitmap_find_next_zero_area(bit_test, 50, idx, 9, 0));

        return 0;
}
static void bitmap_exit(void)
{
	pr_info("--- Bye the cruel world---\n");
}
module_init(bitmap_init);
module_exit(bitmap_exit);
