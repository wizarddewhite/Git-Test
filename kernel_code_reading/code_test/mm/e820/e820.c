#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "e820.h"

static struct e820_table e820_table_init;
struct e820_table *e820_table = &e820_table_init;

static void e820_print_type(enum e820_type type)
{
	switch (type) {
	case E820_TYPE_RAM:		/* Fall through: */
	case E820_TYPE_RESERVED_KERN:	printf("usable");			break;
	case E820_TYPE_RESERVED:	printf("reserved");			break;
	case E820_TYPE_SOFT_RESERVED:	printf("soft reserved");		break;
	case E820_TYPE_ACPI:		printf("ACPI data");			break;
	case E820_TYPE_NVS:		printf("ACPI NVS");			break;
	case E820_TYPE_UNUSABLE:	printf("unusable");			break;
	case E820_TYPE_PMEM:		/* Fall through: */
	case E820_TYPE_PRAM:		printf("persistent (type %u)", type);	break;
	default:			printf("type %u", type);		break;
	}
}

void e820__print_table(char *who)
{
	int i;

	for (i = 0; i < e820_table->nr_entries; i++) {
		printf("%s: [mem %#018Lx-%#018Lx] ",
			who,
			e820_table->entries[i].addr,
			e820_table->entries[i].addr + e820_table->entries[i].size - 1);

		e820_print_type(e820_table->entries[i].type);
		printf("\n");
	}
}

/*
 * Add a memory region to the kernel E820 map.
 */
static void __e820__range_add(struct e820_table *table, u64 start, u64 size, enum e820_type type)
{
	int x = table->nr_entries;

	if (x >= ARRAY_SIZE(table->entries)) {
		printf("too many entries; ignoring [mem %#010llx-%#010llx]\n",
		       start, start + size - 1);
		return;
	}

	table->entries[x].addr = start;
	table->entries[x].size = size;
	table->entries[x].type = type;
	table->nr_entries++;
}

void e820__range_add(u64 start, u64 size, enum e820_type type)
{
	__e820__range_add(e820_table, start, size, type);
}
