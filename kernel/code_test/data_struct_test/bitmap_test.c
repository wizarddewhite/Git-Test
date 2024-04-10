#include <stdio.h>
#include "types.h"
#include "bitmap.h"
#include "bitops.h"
#include "test_common.h"

#define TESTS_BITS 35
DECLARE_BITMAP(bitmaps, TESTS_BITS);

void bitmap_zero_checks()
{
	int i;

	prefix_reset();
	prefix_push("bitmap_zero");
	test_print("Running %s tests...\n", "bitmap_zero");

	PREFIX_PUSH();

	bitmap_zero(bitmaps, TESTS_BITS);

	for(i = 0; i < TESTS_BITS; i++)
		ASSERT_FALSE(test_bit(i, bitmaps));

	test_pass_pop();

	prefix_pop();
}

void bitmap_fill_checks()
{
	int i;

	prefix_reset();
	prefix_push("bitmap_fill");
	test_print("Running %s tests...\n", "bitmap_fill");

	PREFIX_PUSH();

	bitmap_fill(bitmaps, TESTS_BITS);

	for(i = 0; i < TESTS_BITS; i++)
		ASSERT_TRUE(test_bit(i, bitmaps));

	test_pass_pop();

	prefix_pop();
}

void bitmap_clear_one_check()
{
	int i;

	PREFIX_PUSH();

	bitmap_fill(bitmaps, TESTS_BITS);

	for(i = 0; i < TESTS_BITS; i++) {
		ASSERT_TRUE(test_bit(i, bitmaps));
		bitmap_clear(bitmaps, i, 1);
		ASSERT_FALSE(test_bit(i, bitmaps));
	}

	test_pass_pop();

}

void bitmap_clear_two_check()
{
	int i;

	PREFIX_PUSH();

	bitmap_fill(bitmaps, TESTS_BITS);

	for(i = 0; i < TESTS_BITS - 1; i +=2 ) {
		ASSERT_TRUE(test_bit(i, bitmaps));
		ASSERT_TRUE(test_bit(i + 1, bitmaps));
		bitmap_clear(bitmaps, i, 2);
		ASSERT_FALSE(test_bit(i, bitmaps));
		ASSERT_FALSE(test_bit(i + 1, bitmaps));
	}

	test_pass_pop();
}

void bitmap_clear_checks()
{
	prefix_reset();
	prefix_push("bitmap_clear");
	test_print("Running %s tests...\n", "bitmap_clear");

	bitmap_clear_one_check();
	bitmap_clear_two_check();

	prefix_pop();
}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);

	bitmap_zero_checks();
	bitmap_fill_checks();
	bitmap_clear_checks();
	return 0;
}
