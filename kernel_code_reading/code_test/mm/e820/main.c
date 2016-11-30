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



int main()
{
	sanitize_bootmap();
	return 0;
}
