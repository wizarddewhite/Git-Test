#include <stdio.h>
#include <stdlib.h>
#include <math.h>

char x1, x2;
int z1, z2;

/* Steps for King */
int king_steps(void)
{
	int d1, d2;
	d1 = abs(x1 - x2); d2 = abs(z1 - z2);
	
	if ((d1 - d2) > 0)
		return d1;
	else
		return d2;
}

int queen_steps(void)
{
	int d1, d2;

	if ((x1 == x2) && (z1 == z2))
		return 0;

	if ((x1 == x2) || (z1 == z2))
		return 1;

	d1 = abs(x1 - x2); d2 = abs(z1 - z2);
	if (d1 == d2)
		return 1;
	else
		return 2;
}

int rook_steps(void)
{
	int steps = 2;

	if (x1 == x2)
		steps--;

	if (z1 == z2)
		steps--;

	return steps;
}

int bishop_steps(void)
{
	int dis;

	dis = abs(x1 - x2) + abs(z1 - z2);
	if (dis % 2)
		return -1;
	if (abs(x1 - x2) == abs(z1 - z2)) {
		if (x1 == x2)
			return 0;
		else
			return 1;
	}
	return 2;
}

int main()
{
	char line[1024];
	int chesses, i;
	int steps;

	if (fgets(line, 1024, stdin))
		sscanf(line, "%d", &chesses);

	for (i = 0; i < chesses; i++) {
		if (fgets(line, 1024, stdin))
			sscanf(line, "%c%d %c%d", &x1, &z1, &x2, &z2);
		//printf("Original: %c,%d, Target: %c,%d\n", x1, z1, x2, z2);
		x1 = 'i' - x1; x2 = 'i' - x2;
		z1 = 9 - z1; z2 = 9 - z2;
		//printf("Original: %d,%d, Target: %d,%d\n", x1, z1, x2, z2);

		printf("%d %d %d ",
			king_steps(),
			queen_steps(),
			rook_steps()
			);

		steps = bishop_steps();
		if (steps == -1)
			printf("Inf\n");
		else
			printf("%d\n", steps);
	}

	return 0;
}
