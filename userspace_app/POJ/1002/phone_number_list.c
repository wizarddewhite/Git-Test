#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define DEBUG 0
#define PRE_ALLOC 1

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

struct phone_number_record {
#if PRE_ALLOC
#else
	struct phone_number_record *next_record;
	int counts;
#endif
	char normal_form[9];
};

struct phone_number_record *phone_numbers = NULL;
int duplicate = 0;

#if PRE_ALLOC
struct phone_number_record phone_number[100000];
int allocated_records = 0;

static inline struct phone_number_record *alloc_record()
{
	return phone_number + allocated_records++;
}

static inline void free_record(struct phone_number_record *record)
{
	allocated_records--;
}

int record_compare(const void *r1, const void *r2)
{
	return strcmp(((struct phone_number_record *)r1)->normal_form,
		      ((struct phone_number_record *)r2)->normal_form);
}
#else
static inline struct phone_number_record *alloc_record()
{
	struct phone_number_record *record;

	record = calloc(sizeof(struct phone_number_record), 1);
	record->counts = 1;

	return record;
}

static inline void free_record(struct phone_number_record *record)
{
	free(record);
}
#endif

struct phone_number_record *memchar_to_record(char *memchar, int len)
{
	char tmp[7] = {0};
	int i, j;
	struct phone_number_record *record;

	record = alloc_record();
	if (!record)
		return NULL;

	for (i = 0, j = 0; i < strlen(memchar) - 1; i++) {
		if (memchar[i] == '-')
			continue;

		if (memchar[i] >= 'A' && memchar[i] <= 'Z') {
			memchar[i] = toupper(memchar[i]);
			/* Q and Z are not valid */
			if (memchar[i] == 'Q' || memchar[i] == 'Z')
				goto fail;
			memchar[i] = alpha_digit_map[memchar[i] - 65];
		}


		record->normal_form[j++] = memchar[i];
		if (j == 3)
			record->normal_form[j++] = '-';

		/* Just has 9 character in total */
		if (j >= 8)
			break;
	}

	/* Just has 9 character in total */
	if (j != 8)
		goto fail;

	record->normal_form[8] = '\0';
#if DEBUG
	printf("\tcreate record %s \n", record->normal_form);
#endif

	return record;

fail:
	free(record);
	return NULL;
}

#if PRE_ALLOC
void insert_record(struct phone_number_record *record) {}
#else
void insert_record(struct phone_number_record *record)
{
	struct phone_number_record **prev;
	int result;

	/* First Insertion */
	if (!phone_numbers) {
		phone_numbers = record;
		return;
	}

	prev = &phone_numbers;
	while (*prev) {
		/* Duplicate record, increase counts and return */
		result = strcmp(record->normal_form, (*prev)->normal_form);
		if (!result) {
#if DEBUG
	printf("\tget dup record %p \n", *prev);
#endif
			(*prev)->counts++;
			duplicate = 1;
			free_record(record);
			return;
		}

		if (result < 0)
			break;
		prev = &(*prev)->next_record;
	}
	
#if DEBUG
	printf("\tinsert new record %p \n", record);
#endif
	record->next_record = *prev;
	*prev = record;
	return;
}
#endif

int main(int argc, char *argv[])
{
	size_t len = 0;
	char line[1024];
	size_t read;
	int entries, i, j;
	struct phone_number_record *record;

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
		record = memchar_to_record(line, read);
		if (!record)
			continue;

		insert_record(record);
	}

#if PRE_ALLOC
	qsort(phone_number, entries, sizeof(struct phone_number_record),
		record_compare);

	i = 0;
	while (i < entries) {
		j = i;
		i++;
		while (i < entries &&
			!strcmp(phone_number[i].normal_form, phone_number[j].normal_form))
			i++;
		if ((i - j) > 1) {
			printf("%s %d\n", phone_number[j].normal_form, i - j);
			duplicate = 1;
		}
	}

	if (!duplicate)
		printf("No duplicates.\n");

	return 0;
#else
	if (!duplicate) {
		printf("No duplicates.\n");
		return 0;
	}

	record = phone_numbers;
	while (record) {
		if (record->counts != 1)
			printf("%s %d\n", record->normal_form, record->counts);
		record = record->next_record;
	}

	return 0;
#endif
}
