#include <stdio.h>

int main()
{
	printf("~~~~~~~~~~~~> Before breakpoint\n");
	// The breakpoint
	printf("~~~~~~~~~~~~> After breakpoint\n");
	sleep(5);
	printf("return from child\n");

	return 0;
}
