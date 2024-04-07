#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "bug.h"

void panic(const char *fmt, ...)
{
	va_list args;
	printf("PANIC: ");
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	exit(1);
}

void warn_slowpath_fmt(const char *file, int line, unsigned taint,
		       const char *fmt, ...)
{
	va_list args;

	printf(CUT_HERE);

	printf("WARNING: at %s:%d \n", file, line);
	if (!fmt) {
		return;
	}

	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}
