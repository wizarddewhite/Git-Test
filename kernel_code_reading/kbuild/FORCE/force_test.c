#include <stdio.h>
#include "force.h"
#include "unforce.h"

int main()
{
	int a, b;

	a = 3, b = 5;

	print(add(a, b));
	print(sub(a, b));

	return 0;
}
