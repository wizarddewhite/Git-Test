#include "lib.h"

extern int print(char *msg);

void __section(".meminit.text") test_func()
{
	print("A test function\n");
}
