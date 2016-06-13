#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MAX_NUMBER 220
char input1[MAX_NUMBER], input2[MAX_NUMBER];
int len1, len2;
unsigned int val1[MAX_NUMBER], val2[MAX_NUMBER];
unsigned int res[MAX_NUMBER * 2];

void print_num_array(unsigned int num_array[MAX_NUMBER])
{
	int i;
	bool zero_skipped = false;
	for (i = MAX_NUMBER - 1; i >= 0; i --) {
		if (zero_skipped)
			printf("%d", num_array[i]);
		else if (num_array[i] != 0) {
			printf("%d", num_array[i]);
			zero_skipped = true;
		}
	}

	/* All 0 case */
	if (!zero_skipped)
		printf("0");

	printf("\n");
}

void get_input()
{
	int i;

	/* Get input two numbers */
	fgets(input1, MAX_NUMBER, stdin);
	fgets(input2, MAX_NUMBER, stdin);

	/* Clear val1 and val2 */
	memset(val1, 0, sizeof(val1));
	memset(val2, 0, sizeof(val2));

	/* Convert string to int array */
	len1 = strlen(input1) - 1;
	for (i = 0; i < len1; i++)
		val1[i] = input1[len1 - 1 - i] - '0';
	len2 = strlen(input2) - 1;
	for (i = 0; i < len2; i++)
		val2[i] = input2[len2 - 1 - i] - '0';

	//print_num_array(val1);
	//print_num_array(val2);
}

/* val1 = val1 - val2 * (10 * shift) */
int minus(int shift)
{
	int i, j, last_non_zero = 0;

	for (i = MAX_NUMBER - 1; i >= 0; i--) {
		if (val2[i - shift] > val1[i]) {
			/*
			 * if val1 < val2 reverse the process
			 *
			 * Can we borrow from higher?
			 */
			if (!last_non_zero)
				goto reverse;
			/* borrow one from last non-zero */
			val1[last_non_zero]--;
			/* add 9 to [j - 1, i + 1] */
			for (j = last_non_zero - 1; j > i; j--)
				val1[j] += 9;
			val1[i] += 10;
		}
		val1[i] -= val2[i - shift];
		if (val1[i])
			last_non_zero = i;

		if ((i - shift) == 0)
			break;
	}

	//printf("minus: ");
	//print_num_array(val1);
	return 0;

reverse:
	/* In case val1 < val2, add back val2 and return -1 */
	for (i += 1; i < MAX_NUMBER; i++) {
		val1[i] += val2[i - shift];
		if (val1[i] >= 10) {
			val1[i] -= 10;
			val1[i + 1]++;
		}
	}
	return -1;
}

unsigned int div()
{
	unsigned int times = 0;
	int shift;

	/* Div2 > Div1 */
	if (len2 > len1 || 
	    (len1 == len2 && val2[len2-1] > val1[len1-1])) {
		printf("0\n");
		return 0;
	}

	/* Calculate proper shift and sub */
	for (shift = len1 - len2; shift >= 0 ; shift--) {
		unsigned int tmp = 0;
		while (!minus(shift))
			tmp++;
		times += tmp * pow(10, shift);
	}

	/* print what left */
	printf("times: %u\n", times);

	return times;
}

int main()
{

	/* Get div value */
	get_input();

	/* Test minus */
	minus(0);

	/* Do div */
	//div();

	return 0;
}
