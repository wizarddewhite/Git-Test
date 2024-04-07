#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "range.h"

int main()
{
	nr_pfn_mapped = 0;
	memset(pfn_mapped, 0, sizeof(pfn_mapped));
	nr_pfn_mapped = add_range_with_merge(pfn_mapped, E820_X_MAX,
			nr_pfn_mapped, 5, 7);
	dump_pfn_mapped();
	return 0;
}

