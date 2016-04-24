#include <stdio.h>

#define P 23
#define E 28
#define I 33

int main()
{
	char line[1024];
	int p, e, i, d;
	int peak, idx;

	idx = 0;
	while (fgets(line, 1024, stdin)) {
		idx++;
		sscanf(line, "%d %d %d %d", &p, &e, &i, &d);
		if (p == -1 && e == -1 && i == -1 && d == -1)
			break;
		p = p % P; e = e % E; i = i % I;
		//printf("%d %d %d %d\n", p, e, i, d);
		for (peak = d + 21252; ; peak--) {
			if ((peak % P) != p)
				continue;
			if ((peak % E) != e)
				continue;
			if ((peak % I) != i)
				continue;
			break;
		}
		printf("Case %d: the next triple peak occurs in %d days.\n",
			idx, peak - d);
	}


	return 0;
}
