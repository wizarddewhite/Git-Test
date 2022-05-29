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
	printf("  MAX_ERRNO = %016lx \n", (unsigned long)MAX_ERRNO);
	printf("- MAX_ERRNO = %lx \n", (unsigned long)(-MAX_ERRNO));
	printf("MAX_ERR ent = %lx \n", (unsigned long)entry);
	entry = xa_mk_internal(-1);
	printf("  MIN_ERRNO = %016lx \n", (unsigned long)1);
	printf("- MIN_ERRNO = %016lx \n", (unsigned long)(-1));
	printf("MIN_ERR ent = %lx \n", (unsigned long)entry);
	printf("error code in entry: %d\n", xa_err(entry));
}

int main()
{
	xa_internal_test();

	return 0;
}
