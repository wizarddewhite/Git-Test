/*
 * =====================================================================================
 *
 *       Filename:  phone_number.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/16/2016 07:13:53 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct phone_number_record {
	int phone_number;
	char normal_form[9];
	int counts;
};

struct phone_number_record *phone_numbers;

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

int memchar_to_number(char *memchar, int len, struct phone_number_record *entry)
{
	char tmp[7] = {0};
	int i, j, number;
	struct phone_number_record *iter;

	for (i = 0, j = 0; i < strlen(memchar) - 1; i++) {
		if (memchar[i] == '-')
			continue;

		if (isalpha(memchar[i])) {
			memchar[i] = toupper(memchar[i]);
			/* Q and Z are not valid */
			if (memchar[i] == 'Q' || memchar[i] == 'Z')
				return -1;
			memchar[i] = alpha_digit_map[memchar[i] - 65];
		}

		tmp[j++] = memchar[i];

		/* Just has 7 character in total */
		if (j >= 7)
			break;
	}

	/* Just has 7 character in total */
	if (j != 7)
		return -1;

	sscanf(tmp, "%d", &number);
	for (iter = phone_numbers; iter < entry; iter++) {
		if (iter->phone_number > number) {
			memmove(iter, iter + 1, sizeof(*iter) * (entry - iter));
			break;
		}

		if (iter->phone_number == number) {
			iter->counts++;
			return 1;
		}
	}

	entry->phone_number = number;
	entry->counts = 1;
	memcpy(entry->normal_form, tmp, 3);
	entry->normal_form[3] = '-';
	memcpy(entry->normal_form + 4, tmp + 3, 4);
	entry->normal_form[8] = '\0';

	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int entries, i;
	struct phone_number_record *iter;
	int duplicate = 0;

	if (argc != 2) {
		printf("No Input File\n");
		exit(-1);
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror("Open File");
		exit(-1);
	}

	/* The first line is the number of entries in the file */
	if ((read = getline(&line, &len, fp) != -1))
		sscanf(line, "%d", &entries);
	else
		exit(-1);

	phone_numbers = calloc(sizeof(*phone_numbers), entries);
	if (!phone_numbers) {
		perror("Phone Number Array allocation failed");
		exit(-1);
	}

	i = 0;
	while ((read = getline(&line, &len, fp) != -1)) {
		if (!memchar_to_number(line, read, phone_numbers + i))
			i++;
		if (i >= entries)
			break;
	}

	for (iter = phone_numbers; ;iter++) {
		if (!iter->counts)
			break;
		if (iter->counts > 1) {
			duplicate = 1;
			printf("%s %d\n", iter->normal_form, iter->counts);
		}
	}

	if (!duplicate)
		printf("No duplicates\n");

	if (line)
		free(line);
	fclose(fp);

	return 0;
}
