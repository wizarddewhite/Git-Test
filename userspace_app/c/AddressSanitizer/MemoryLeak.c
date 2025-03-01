#include <stdio.h>
#include <stdlib.h>

char* getMemory()
{
	char *p = (char *)malloc(30);
	return p;
}

int main()
{
	char *p = getMemory();
	// Fix memory leak by free it before exit
	// free(p);
	p = NULL;

	return 0;
}
