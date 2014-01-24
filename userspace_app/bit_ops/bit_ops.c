/*
 * =====================================================================================
 *
 *       Filename:  bit_ops.c
 *
 *    Description:  This file focus on common bit operations.
 *
 *        Version:  1.0
 *        Created:  01/24/2014 10:30:23 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdbool.h>

bool is_power2(int x)
{
	if (x == 0)
		return false;

	if ((x & (x-1)) == 0)
		return true;
	else
		return false;
}

void test_is_power2()
{
	printf("%d is%spower of 2\n",
			2, is_power2(2)?" ":" not ");
	printf("%d is%spower of 2\n",
			5, is_power2(5)?" ":" not ");
	printf("%d is%spower of 2\n",
			8, is_power2(8)?" ":" not ");
	printf("%d is%spower of 2\n",
			16, is_power2(16)?" ":" not ");
	printf("%d is%spower of 2\n",
			96, is_power2(96)?" ":" not ");
}

/*
 * find last (most-significant) bit set
 *
 * Note fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 */
int fls(int x)
{
	int r = 32;

	if (!x)
		return 0;
	if (!(x & 0xffff0000u)) {
		x <<= 16;
		r -= 16;
	}
	if (!(x & 0xff000000u)) {
		x <<= 8;
		r -= 8;
	}
	if (!(x & 0xf0000000u)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & 0xc0000000u)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & 0x80000000u)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}

/*
 * ffs - find first bit set
 *
 * Note ffs(0) = 0, ffs(1) = 1, ffs(0x80000000) = 32.
 */
int ffs(int x)
{
	int r = 1;

	if (!x)
		return 0;
	if (!(x & 0xffff)) {
		x >>= 16;
		r += 16;
	}
	if (!(x & 0xff)) {
		x >>= 8;
		r += 8;
	}
	if (!(x & 0xf)) {
		x >>= 4;
		r += 4;
	}
	if (!(x & 3)) {
		x >>= 2;
		r += 2;
	}
	if (!(x & 1)) {
		x >>= 1;
		r += 1;
	}
	return r;
}

void test_ffs_fls()
{
	printf("first bit set in %x is %d\n", 0x120, ffs(0x120));
	printf("first bit set in %x is %d\n", 0x110, ffs(0x110));
	printf("first bit set in %x is %d\n", 0x100, ffs(0x100));

	printf("\n");

	printf("last bit set in %x is %d\n", 0x020, fls(0x021));
	printf("last bit set in %x is %d\n", 0x100, fls(0x180));
	printf("last bit set in %x is %d\n", 0x800, fls(0x840));
}

int power2_roundup(int x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

void test_power2_roundup()
{
	printf("%d power2 roundup is %d\n",
			5, power2_roundup(5));
	printf("%d power2 roundup is %d\n",
			126, power2_roundup(126));
}

int main()
{
	test_ffs_fls();
	return 0;
}
