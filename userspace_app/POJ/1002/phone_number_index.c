#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define DEBUG 0

char alpha_digit_map[] = {
	'2',   /* 'A' */
	'2',   /* 'B' */
	'2',   /* 'C' */
	'3',   /* 'D' */
	'3',   /* 'E' */
	'3',   /* 'F' */
	'4',   /* 'G' */
	'4',   /* 'H' */
	'4',   /* 'I' */
	'5',   /* 'J' */
	'5',   /* 'K' */
	'5',   /* 'L' */
	'6',   /* 'M' */
	'6',   /* 'N' */
	'6',   /* 'O' */
	'7',   /* 'P' */
	'0',   /* 'Q' */
	'7',   /* 'R' */
	'7',   /* 'S' */
	'8',   /* 'T' */
	'8',   /* 'U' */
	'8',   /* 'V' */
	'9',   /* 'W' */
	'9',   /* 'X' */
	'9',   /* 'Y' */
	'0',   /* 'Z' */
};


int duplicate = 0;

struct phone_number_record {
	unsigned int counts;
};
#define MAX_VALUE 10000000
struct phone_number_record phone_numbers[MAX_VALUE];

static inline void memchar_to_record(char *memchar, int len)
{
	int i, j;
	int number = 0;

	for (i = 0, j = 0; j < 7; i++) {
		if (memchar[i] == '-')
			continue;

		if (memchar[i] >= 'A' && memchar[i] <= 'Z')
			memchar[i] = alpha_digit_map[memchar[i] - 65];

		j++;
		number = number * 10 + memchar[i] - 48;
	}

#if DEBUG
	printf("\tcreate record %03u-%04u \n", 
				number / 10000,
				number % 10000);
#endif
	phone_numbers[number].counts++;

}

int main(int argc, char *argv[])
{
	size_t len = 0;
	char line[1024];
	size_t read;
	int entries, i;

	/* The first line is the number of entries in the file */
	if (fgets(line, 1024, stdin))
		sscanf(line, "%d", &entries);
	else
		exit(-1);

#if DEBUG
	printf("File has %d entries\n", entries);
#endif

	while (fgets(line, 1024, stdin)) {
#if DEBUG
		printf("File readline %s ", line);
#endif
		memchar_to_record(line, read);
	}

	i = 0;
	while (i < MAX_VALUE) {
		if (phone_numbers[i].counts > 1) {
			printf("%03d-%04d %d\n",
				i / 10000,
				i % 10000,
				phone_numbers[i].counts );
			duplicate = 1;
		}
		i++;
	}

	if (!duplicate)
		printf("No duplicates.\n");

	return 0;
}
