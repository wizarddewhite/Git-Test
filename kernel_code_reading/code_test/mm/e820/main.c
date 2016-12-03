#include <stdio.h>
#include "e820.h"

void init_bootmap()
{
	bootmap.nr_map = 2;
	bootmap.map[0].addr = 0x1000;
	bootmap.map[0].size = 0x1000;
	bootmap.map[0].type = E820_RAM;
	bootmap.map[1].addr = 0x2000;
	bootmap.map[1].size = 0x1000;
	bootmap.map[1].type = E820_UNUSABLE;

	e820_print_map("bootmap", &bootmap);
}

void sanitize_bootmap()
{
	u32 new_nr;

	bootmap.nr_map = 2;
	bootmap.map[0].addr = 0x1000;
	bootmap.map[0].size = 0x2000;
	bootmap.map[0].type = E820_RAM;
	bootmap.map[1].addr = 0x2000;
	bootmap.map[1].size = 0x1000;
	bootmap.map[1].type = E820_UNUSABLE;

	printf("--- origin ---\n");
	e820_print_map("bootmap", &bootmap);

	new_nr = bootmap.nr_map;
	sanitize_e820_map(bootmap.map, E820_X_MAX, &new_nr);
	printf("--- sanitized ---\n");
	e820_print_map("bootmap", &bootmap);
}

void append_to_e820()
{
	sanitize_bootmap();

	append_e820_map(bootmap.map, bootmap.nr_map);
	printf("--- append to e820 ---\n");
	e820_print_map("e820", &e820);
}

void get_end_pfn()
{
	append_to_e820();
	printf("end_pfn: %#018Lx\n", e820_end_pfn(MAX_ARCH_PFN));
	e820_add_region((u64)0x5000, (u64)(MAXMEM - 0x5000), E820_RAM);
	e820_print_map("e820", &e820);
	printf("end_pfn: %#018Lx\n", e820_end_pfn(MAX_ARCH_PFN));
}

void add_remove_range()
{
	e820_add_region((u64)0x1000, (u64)(0x1000), E820_RAM);
	e820_add_region((u64)0x3000, (u64)(0x2000), E820_UNUSABLE);
	e820_add_region((u64)0x5000, (u64)(0x5000), E820_RAM);
	e820_add_region((u64)0x10000, (u64)(0x5000), E820_RAM);
	e820_print_map("e820", &e820);

	/* remote a total range */
	e820_remove_range(&e820, 0x5000, 0x5000, E820_RAM, 1);
	e820_print_map("e820", &e820);

	/* remote a partial range */
	e820_remove_range(&e820, 0x11000, 0x2000, E820_RAM, 1);
	e820_print_map("e820", &e820);

	/* remote a head range */
	e820_remove_range(&e820, 0x13000, 0x1000, E820_RAM, 1);
	e820_print_map("e820", &e820);
}

int main()
{
	add_remove_range();
	return 0;
}
