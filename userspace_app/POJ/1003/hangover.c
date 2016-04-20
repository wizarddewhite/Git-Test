#include <stdio.h>

float hangover(int n)
{
	float i, sum = 0;

	for (i=1; i<=n; i++)
		sum += 1/(i + 1);

	return sum;
}

int largest_hangover(float ceil)
{
	int i;

	for (i=1; i<10000;i++)
		if (hangover(i) > ceil)
			return i;
	return 0;
}

int main()
{
	char line[1024];
	float ceil;

next:
	if (fgets(line, 1024, stdin))
		sscanf(line, "%f", &ceil);

	if (ceil == 0.00)
		return 0;

	printf("%d card(s)\n", largest_hangover(ceil));
	goto next;
	return 0;
}
