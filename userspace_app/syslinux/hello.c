// gcc -static -o hello hello.c
// echo hello | cpio -o --format=newc > rootfs

#include <stdio.h>
void main()
{
	printf("Hello World\n");
	fflush(stdout);
	while(1);
}
