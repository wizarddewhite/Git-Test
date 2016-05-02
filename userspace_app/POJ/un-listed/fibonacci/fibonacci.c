#include <stdio.h>

int f(int n)
{
	if (n == 1 || n == 2)
		return 1;
	return f(n - 1) + f(n - 2);
}

int main()
{
	char line[1024];
	int lines, num;

	fgets(line, 1024, stdin);
	sscanf(line, "%d", &lines);

	for (;lines > 0; lines--) {
		fgets(line, 1024, stdin);
		sscanf(line, "%d", &num);
		printf("%d\n", f(num));
	}
	return 0;
}
