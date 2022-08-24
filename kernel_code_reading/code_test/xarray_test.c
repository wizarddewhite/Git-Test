#include <stdio.h>
#include <stdlib.h>

#include "xarray.h"
#include "bug.h"

static void xa_internal_test()
{
	void *entry, *entry1;
	int *integer;

	entry = xa_mk_internal(10);
	printf("val   = %ld\n", xa_to_internal(entry));
	printf("entry = %ld = %ld * 4 + 2\n", (unsigned long)entry, xa_to_internal(entry));

	printf("entry + 1 = %ld\n", (unsigned long)(entry + 1));
	integer = entry;
	printf("integer = %ld\n", (unsigned long)integer);
	printf("integer + 1 = %ld\n", (unsigned long)(integer + 1));

	entry1 = entry + 10;
	printf("entry1 = entry + 10 = %ld \n", (unsigned long)entry1);
	printf("entry1 is %s than entry\n", entry1 > entry ? "bigger":"smaller");

	entry = xa_mk_internal(-MAX_ERRNO);
	if (!xa_is_err(entry))
		panic("%016lx is not a valid errno\n", (unsigned long)(-MAX_ERRNO));
	printf("  MAX_ERRNO = %016lx \n", (unsigned long)MAX_ERRNO);
	printf("- MAX_ERRNO = %lx \n", (unsigned long)(-MAX_ERRNO));
	printf("MAX_ERR ent = %lx \n", (unsigned long)entry);
	entry = xa_mk_internal(-1);
	if (!xa_is_err(entry))
		panic("%016lx is not a valid errno\n", (unsigned long)(-1));
	printf("  MIN_ERRNO = %016lx \n", (unsigned long)1);
	printf("- MIN_ERRNO = %016lx \n", (unsigned long)(-1));
	printf("MIN_ERR ent = %lx \n", (unsigned long)entry);
	printf("error code in entry: %d\n", xa_err(entry));

	printf("sizeof(xa_node): %ld\n", sizeof(struct xa_node));

	xa_is_sibling(entry);

	printf("right shift (unsigned long)-1: %lx\n", (unsigned long)(-1) >> 1);
	printf("right shift (long)-1: %lx\n", (long)(-1) >> 1);
}

void xas_movement()
{
	unsigned long offset = 0, index = 0;
	printf("   XA_CHUNK_MASK     : %016lx\n", XA_CHUNK_MASK);
	printf("  ~XA_CHUNK_MASK     : %016lx\n", ~XA_CHUNK_MASK);
	printf("  ~XA_CHUNK_MASK << 4: %016lx\n", ~XA_CHUNK_MASK << 4);
	printf("~(XA_CHUNK_MASK << 4): %016lx\n", ~(XA_CHUNK_MASK << 4));

	// copied from xas_move_index
	// this would clear bits in [0, shift]
	index &= ~XA_CHUNK_MASK << 0;
	index += offset << 0;
	printf("current index: %016lx\n", index);
	offset++;
	index &= ~XA_CHUNK_MASK << 0;
	index += offset << 0;
	printf("move to index: %016lx\n", index);

	index = 0xc49; offset = 4;
	printf("current index: %016lx\n", index);
	printf("   next index: %016lx\n", index + 1);
	offset++;
	index &= ~XA_CHUNK_MASK << 4;
	index += offset << 4;
	printf("move to index: %016lx\n", index);

}

static void *xa_mk_index(unsigned long index)
{
	return xa_mk_value(index & LONG_MAX);
}

static void *xa_store_index(struct xarray *xa, unsigned long index, gfp_t gfp)
{
	return xa_store(xa, index, xa_mk_index(index), gfp);
}

void check_xa_store()
{
	DEFINE_XARRAY(array);
	xa_store_index(&array, 0, 0);
	xa_store_index(&array, 1, 0);
	xa_store_index(&array, 0xff, 0);
	xa_dump(&array, false);
}

static void *xa_store_order(struct xarray *xa, unsigned long index,
		unsigned order, void *entry, gfp_t gfp)
{
	XA_STATE_ORDER(xas, xa, index, order);
	void *curr;

	do {
		xas_lock(&xas);
		curr = xas_store(&xas, entry);
		xas_unlock(&xas);
	} while (xas_nomem(&xas, gfp));

	return curr;
}

void check_multi_order()
{
	DEFINE_XARRAY(array);
	xa_store_order(&array, 0, 1, xa_mk_value(0), 0);
	xa_dump(&array, false);
	xa_store_order(&array, 1, 1, xa_mk_value(1), 0);
	xa_dump(&array, false);
}

// expand the order
void check_multi_order2()
{
	void *old;

	DEFINE_XARRAY(array);
	old = xa_store_order(&array, 2, 1, xa_mk_value(0), 0);
	printf("The old value %p\n", old);
	xa_dump(&array, false);
	old = xa_store_order(&array, 2, 2, xa_mk_value(0), 0);
	printf("The old value %p\n", old);
	xa_dump(&array, false);
}

unsigned long xas_size(const struct xa_state *xas);
unsigned long xas_max(struct xa_state *xas);
void check_xas_max()
{
	XA_STATE_ORDER(xas, NULL, 0xffffffffffffffff, 3);
	printf("xas_size: %lx\n", xas_size(&xas));
	printf("max: %lx\n", xas_max(&xas));
	xas.xa_index = 7;
	printf("max: %lx\n", xas_max(&xas));
}

static void __check_store_range(struct xarray *xa, unsigned long first,
		unsigned long last)
{
#ifdef CONFIG_XARRAY_MULTI
	xa_store_range(xa, first, last, xa_mk_index(first), 0);

	XA_BUG_ON(xa, xa_load(xa, first) != xa_mk_index(first));
	XA_BUG_ON(xa, xa_load(xa, last) != xa_mk_index(first));
	XA_BUG_ON(xa, xa_load(xa, first - 1) != NULL);
	XA_BUG_ON(xa, xa_load(xa, last + 1) != NULL);

	xa_store_range(xa, first, last, NULL, 0);
#endif

	XA_BUG_ON(xa, !xa_empty(xa));
}

static void check_store_range()
{
	DEFINE_XARRAY(xa);
	unsigned long i, j;

	__check_store_range(&xa, 1, 10);
	xa_store_range(&xa, 2, 64, xa_mk_index(2), 0);
	xa_dump(&xa, false);

	/*
	 * Then let's just store value to one entry.
	 * This would keep the range set above.
	 */
	xa_store(&xa, 32, xa_mk_value(3), 0);
	xa_dump(&xa, false);
#if 0
	for (i = 0; i < 128; i++) {
		for (j = i; j < 128; j++) {
			__check_store_range(&xa, i, j);
			__check_store_range(&xa, 128 + i, 128 + j);
			__check_store_range(&xa, 4095 + i, 4095 + j);
			__check_store_range(&xa, 4096 + i, 4096 + j);
			__check_store_range(&xa, 123456 + i, 123456 + j);
			__check_store_range(&xa, (1 << 24) + i, (1 << 24) + j);
		}
	}
#endif
}

void xas_set_range(struct xa_state *xas, unsigned long first,
		unsigned long last);
void check_set_range()
{
	unsigned long first, last;
	DEFINE_XARRAY(xa);
	XA_STATE(xas, &xa, 0);

	/*
	 * last  1111 1111
	 * first    1 0000
	 */
	first = 0x023, last = 0x2e;
	xas_set_range(&xas, first, last);
	printf("first: %lx, last: %lx\n", first, last);
	printf("  xa->shift: %u, xa->xa_sibs: %u\n", xas.xa_shift, xas.xa_sibs);
	printf("  xas_size: %lx\n", xas_size(&xas));
	printf("  range: [%lx, %lx]\n", first, first + xas_size(&xas) - 1);

	/*
	 * last  1111 1111
	 * first         0
	 */
	first = 0x000, last = 0x1ff;
	xas_set_range(&xas, first, last);
	printf("first: %lx, last: %lx\n", first, last);
	printf("  xa->shift: %u, xa->xa_sibs: %u\n", xas.xa_shift, xas.xa_sibs);
	printf("  xas_size: %lx\n", xas_size(&xas));
	printf("  range: [%lx, %lx]\n", first, first + xas_size(&xas) - 1);

	first = 0x000, last = 0xfe;
	xas_set_range(&xas, first, last);
	printf("first: %lx, last: %lx\n", first, last);
	printf("  xa->shift: %u, xa->xa_sibs: %u\n", xas.xa_shift, xas.xa_sibs);
	printf("  xas_size: %lx\n", xas_size(&xas));
	printf("  range: [%lx, %lx]\n", first, first + xas_size(&xas) - 1);

	first = 0x000, last = 0x20fe;
	xas_set_range(&xas, first, last);
	printf("first: %lx, last: %lx\n", first, last);
	printf("  xa->shift: %u, xa->xa_sibs: %u\n", xas.xa_shift, xas.xa_sibs);
	printf("  xas_size: %lx\n", xas_size(&xas));
	printf("  range: [%lx, %lx]\n", first, first + xas_size(&xas) - 1);
}

void check_xas_split()
{
	unsigned int old_order = 3;
	unsigned int new_order = 2;
	DEFINE_XARRAY(xa);
	XA_STATE_ORDER(xas, &xa, 4, new_order);

	xa_store_order(&xa, 0, old_order, xa_mk_value(5), 0);
	xa_dump(&xa, false);
	xas_split_alloc(&xas, xa_mk_value(3), old_order, 0);
	xas_split(&xas, xa_mk_value(3), old_order);
	xa_dump(&xa, false);
}

void check_create_range()
{
	unsigned long index = 0;
	unsigned int order = 6;
	DEFINE_XARRAY(xa);
	XA_STATE_ORDER(xas, &xa, index, order);

	xas_create_range(&xas);
	xa_dump(&xa, true);
}

void check_align_1()
{
	char text[] = "Linux";
	printf("text: %p\n", text);
	DEFINE_XARRAY(array);
	xa_store(&array, 0, text, 0);
	xa_dump(&array, false);
}

void check_xa_erase()
{
	DEFINE_XARRAY(array);

	xa_store_index(&array, 0, 0);
	xa_store_index(&array, 1, 0);
	xa_dump(&array, false);

	xa_erase(&array, 1);
	xa_dump(&array, false);
}

void check_xa_mark()
{
	DEFINE_XARRAY(xa);
	xa_store_index(&xa, 0, 0);
	xa_set_mark(&xa, 0, XA_MARK_0);
	xa_store_index(&xa, 1, 0);
	xa_set_mark(&xa, 1, XA_MARK_0);
	xa_set_mark(&xa, 1, XA_MARK_1);
	xa_dump(&xa, false);

	xa_store_order(&xa, 0, 2, xa_mk_index(0), 0);
	xa_dump(&xa, false);
}

static DEFINE_XARRAY_ALLOC(xa0);
static DEFINE_XARRAY_ALLOC1(xa1);

void check_xa_alloc()
{
	unsigned int i, id;
	unsigned long index;
	void *entry;
	for (i = 0; i < 10; i++)
		xa_alloc(&xa0, &id, NULL, xa_limit_32b, 0);
	xa_dump(&xa0, false);
	xa_store(&xa0, 3, xa_mk_index(3), 0);
	xa_store(&xa0, 4, xa_mk_index(4), 0);
	xa_dump(&xa0, false);
	xa_store(&xa0, 4, NULL, 0);
	xa_dump(&xa0, false);
	// difference between store NULL and erase?
	xa_erase(&xa0, 5);
	xa_dump(&xa0, false);

	// xa_for_each would skip ZERO entry
	xa_for_each(&xa0, index, entry) {
		printf("find entry %p at %lu\n", entry, index);
		// xa_erase_index(&xa0, index);
	}
}

void check_xa_alloc1()
{
	unsigned int i, id;
	unsigned long index;

	xa_dump(&xa1, false);
	xa_alloc(&xa1, &id, xa_mk_index(1), xa_limit_32b, 0);
	xa_dump(&xa1, false);
	xa_erase(&xa1, 1);
	xa_dump(&xa1, false);
}

void check_xas_next()
{
	void *entry;
	DEFINE_XARRAY(xa);
	XA_STATE(xas, &xa, 0);

	xa_store_index(&xa, 0, 0);
	xas_set(&xas, 0);
	entry = xas_next(&xas);
	printf("index: %lu %p\n", xas.xa_index, entry);
	entry = xas_next(&xas);
	printf("index: %lu %p\n", xas.xa_index, entry);
	entry = xas_next(&xas);
	printf("index: %lu %p\n", xas.xa_index, entry);
}

int main()
{
	// xa_internal_test();
	// xas_next_entry(NULL, 1);
	// xas_movement();
	// check_xa_store();
	// check_multi_order();
	// check_multi_order2();
	// check_xas_max();
	// check_store_range();
	// check_set_range();
	// check_xas_split();
	// check_create_range();
	// check_align_1();
	// check_xa_erase();
	// check_xa_mark();
	// check_xa_alloc();
	// check_xa_alloc1();
	check_xas_next();

	return 0;
}
