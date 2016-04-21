#include <stdio.h>

int main()
{
	char line[1024];
	int choices, i;
	float x, y;

	if (fgets(line, 1024, stdin))
		sscanf(line, "%d", &choices);

	for (i = 0; i < choices; i++) {
		if (fgets(line, 1024, stdin))
			sscanf(line, "%f %f", &x, &y);
		printf("Property %d: This property will begin eroding in year %d.\n",
			i+1, (int)((x*x + y*y) * 3.1415 / 2)/50 + 1);
	}
	printf("END OF OUTPUT.\n");

	return 0;
}
