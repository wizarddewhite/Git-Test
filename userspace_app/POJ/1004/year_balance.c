#include <stdio.h>

int main()
{
	char line[1024];
	int i;
	float mon_balance, year_balance;

	year_balance = 0;
	for (i = 0; i < 12; i++) {
		if (fgets(line, 1024, stdin))
			sscanf(line, "%f", &mon_balance);
		year_balance += mon_balance;
	}

	printf("$%.2f\n", year_balance/12);
	return 0;
}
