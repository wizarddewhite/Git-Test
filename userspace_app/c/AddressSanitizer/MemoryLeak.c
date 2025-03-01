#include <stdio.h>
#include <stdlib.h>

char* getMemory()
{
	char *p = (char *)malloc(30);
	return p;
}

static void *mem;

int main()
{
	char *p = getMemory();
	// Fix memory leak by free it before exit
	// free(p);
	p = NULL;

	// but allocate to a global memory will not detect memory leak
	mem = malloc(10);

	return 0;
}
