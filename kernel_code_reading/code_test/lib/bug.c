#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

void panic(const char *fmt, ...)
{
	va_list args;
	printf("PANIC: ");
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	exit(1);
}

