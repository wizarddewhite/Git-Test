#include <stdio.h>
#include "e820.h"

/*
void check_all_mapped()
{
	u32 new_nr;
	// e820_add_region((u64)0x1000, (u64)(0x1000), E820_RAM);
	// e820_add_region((u64)0x10000, (u64)(0x5000), E820_RAM);
	// e820_add_region((u64)0x3000, (u64)(0x2000), E820_UNUSABLE);
	// e820_add_region((u64)0x5000, (u64)(0x5000), E820_RAM);

	sanitize_e820_map(e820.map, E820_X_MAX, &e820.nr_map);
	e820_print_map("e820", &e820);

	printf("region %#llx-%#llx is%s mapped\n",
		(u64)0x1000, (u64)(0x1000 + 0x500 - 1),
		e820_all_mapped(0x1000, 0x1000 + 0x500 - 1, E820_RAM)?"":"n't");
	printf("region %#llx-%#llx is%s mapped\n",
		(u64)0x1000, (u64)(0x1000 + 0x1000 - 1),
		e820_all_mapped(0x1000, 0x1000 + 0x1000 - 1, E820_RAM)?"":"n't");
	printf("region %#llx-%#llx is%s mapped\n",
		(u64)0x1000, (u64)(0x1000 + 0x1100 - 1),
		e820_all_mapped(0x1000, 0x1000 + 0x1100 - 1, E820_RAM)?"":"n't");
	printf("region %#llx-%#llx is%s mapped\n",
		(u64)0x1100, (u64)(0x1100 + 0x1000 - 1),
		e820_all_mapped(0x1100, 0x1100 + 0x1000 - 1, E820_RAM)?"":"n't");
	printf("region %#llx-%#llx is%s mapped\n",
		(u64)0x2000, (u64)(0x2000 + 0x1000 - 1),
		e820_all_mapped(0x2000, 0x2000 + 0x1000 - 1, E820_RAM)?"":"n't");
}
 */

void add_range()
{
	e820__range_add((u64)0x1000, (u64)(0x1000), E820_TYPE_RAM);
	e820__range_add((u64)0x10000, (u64)(0x5000), E820_TYPE_RAM);
	e820__range_add((u64)0x3000, (u64)(0x2000), E820_TYPE_UNUSABLE);
	e820__range_add((u64)0x5000, (u64)(0x5000), E820_TYPE_RAM);
	e820__print_table("e820");

	e820__update_table(e820_table);
	printf("=== after sanitize ===");
	e820__print_table("e820");
}

void sanitize_range()
{
	/* mimic the range in comment  */
	e820__range_add((u64)0x1020, (u64)(0x0020), E820_TYPE_NVS);
	e820__range_add((u64)0x1010, (u64)(0x0100), E820_TYPE_ACPI);
	e820__range_add((u64)0x1100, (u64)(0x0020), E820_TYPE_RESERVED);
	e820__range_add((u64)0x1000, (u64)(0x1000), E820_TYPE_RAM);

	e820__update_table2(e820_table);
	e820__print_table("e820");
}

void sanitize_range2()
{
	/* mimic the range in comment  */
	e820__range_add((u64)0x1000, (u64)(0x0100), E820_TYPE_RAM);
	e820__range_add((u64)0x10a0, (u64)(0x00f0), E820_TYPE_RESERVED);
	e820__range_add((u64)0x10d0, (u64)(0x0020), E820_TYPE_ACPI);

	e820__update_table2(e820_table);
	e820__print_table("e820");
}

void sanitize_range3()
{
	/* no overlap range  */
	e820__range_add((u64)0x1000, (u64)(0x0100), E820_TYPE_RAM);
	e820__range_add((u64)0x20a0, (u64)(0x00f0), E820_TYPE_RESERVED);
	e820__range_add((u64)0x30d0, (u64)(0x0020), E820_TYPE_ACPI);

	e820__update_table2(e820_table);
	e820__print_table("e820");
}

void range_update()
{
	e820__range_add((u64)0x1000, (u64)(0x0100), E820_TYPE_RAM);
	e820__range_add((u64)0x20a0, (u64)(0x0100), E820_TYPE_RESERVED);
	e820__range_add((u64)0x30d0, (u64)(0x0020), E820_TYPE_ACPI);
	e820__print_table("e820");

	e820__range_update(0x20b0, 0x20, E820_TYPE_RESERVED, E820_TYPE_ACPI);
	printf("=== After update range ===\n");
	e820__print_table("e820");

	e820__update_table2(e820_table);
	printf("=== After update table ===\n");
	e820__print_table("e820");
}

int main()
{
	// add_range();
	// sanitize_range();
	// sanitize_range2();
	// sanitize_range3();
	range_update();
	return 0;
}
