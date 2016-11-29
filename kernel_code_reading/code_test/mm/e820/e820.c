#include <stdio.h>
#include "e820.h"

struct e820map bootmap;
struct e820map e820;
struct e820map e820_saved;

static void e820_print_type(__u32 type)
{
	switch (type) {
	case E820_RAM:
	case E820_RESERVED_KERN:
		printf("usable");
		break;
	case E820_RESERVED:
		printf("reserved");
		break;
	case E820_ACPI:
		printf("ACPI data");
		break;
	case E820_NVS:
		printf("ACPI NVS");
		break;
	case E820_UNUSABLE:
		printf("unusable");
		break;
	case E820_PMEM:
	case E820_PRAM:
		printf("persistent (type %u)", type);
		break;
	default:
		printf("type %u", type);
		break;
	}
}

void e820_print_map(char *who, struct e820map *e820)
{
	int i;

	for (i = 0; i < e820->nr_map; i++) {
		printf("%s: [mem %#018Lx-%#018Lx] ", who,
		       (unsigned long long) e820->map[i].addr,
		       (unsigned long long)
		       (e820->map[i].addr + e820->map[i].size - 1));
		e820_print_type(e820->map[i].type);
		printf("\n");
	}
}

