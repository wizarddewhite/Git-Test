#include <stdio.h>

extern int foo(void);

int main(void)
{
	printf("Hello World!\n");
	printf("Foo = %d\n", foo());
	return 0;
}

