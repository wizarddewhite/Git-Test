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

int main()
{
	init_bootmap();
	return 0;
}
