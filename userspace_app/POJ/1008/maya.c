#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num;
int day, mon, year;
unsigned long days;

int mon_string_to_mon(char *mon_string)
{
	if (!strcmp("pop", mon_string))
		return 0;
	if (!strcmp("no", mon_string))
		return 1;
	if (!strcmp("zip", mon_string))
		return 2;
	if (!strcmp("zotz", mon_string))
		return 3;
	if (!strcmp("tzec", mon_string))
		return 4;
	if (!strcmp("xul", mon_string))
		return 5;
	if (!strcmp("yoxkin", mon_string))
		return 6;
	if (!strcmp("mol", mon_string))
		return 7;
	if (!strcmp("chen", mon_string))
		return 8;
	if (!strcmp("yax", mon_string))
		return 9;
	if (!strcmp("zac", mon_string))
		return 10;
	if (!strcmp("ceh", mon_string))
		return 11;
	if (!strcmp("mac", mon_string))
		return 12;
	if (!strcmp("kankin", mon_string))
		return 13;
	if (!strcmp("muan", mon_string))
		return 14;
	if (!strcmp("pax", mon_string))
		return 15;
	if (!strcmp("koyab", mon_string))
		return 16;
	if (!strcmp("cumhu", mon_string))
		return 17;
	if (!strcmp("uayet", mon_string))
		return 18;
}

char *Tzolkin_day[20] = {
	[0]  = "imix",
	[1]  = "ik",
	[2]  = "akbal",
	[3]  = "kan",
	[4]  = "chicchan",
	[5]  = "cimi",
	[6]  = "manik",
	[7]  = "lamat",
	[8]  = "muluk",
	[9]  = "ok",
	[10] = "chuen",
	[11] = "eb",
	[12] = "ben",
	[13] = "ix",
	[14] = "mem",
	[15] = "cib",
	[16] = "caban",
	[17] = "eznab",
	[18] = "canac",
	[19] = "ahau",
};

int main()
{
	char line[1024];
	char mon_string[6] = {0};
	int i;

	fgets(line, 1024, stdin);
	sscanf(line, "%d\n", &num);
	printf("%d\n", num);

	for (i = 0; i < num; i++) {
		fgets(line, 1024, stdin);
		sscanf(line, "%d. %s %d\n", &day, mon_string, &year);
		mon = mon_string_to_mon(mon_string);
		//printf("%d. %d %d\n", day, mon, year);
		days = day;
		days += 20 * mon;
		days += 365 * year;
		//printf("\ttotal days: %lu.\n", days);

		year = days / (13 * 20);
		days = days % (13 * 20);
		mon = days % 20;
		day = days % 13;
		printf("%d %s %d\n", day + 1, Tzolkin_day[mon], year);
	}
	
	return 0;
}
