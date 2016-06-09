#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NUMBER 220
char input1[MAX_NUMBER], input2[MAX_NUMBER];
int len1, len2;
unsigned int mul1[MAX_NUMBER], mul2[MAX_NUMBER];
unsigned int res[MAX_NUMBER * 2];

void print_num_array(unsigned int num_array[MAX_NUMBER * 2], int len)
{
	int i;
	bool zero_skipped = false;
	for (i = len - 1; i >= 0; i --) {
		if (zero_skipped)
			printf("%u", num_array[i]);
		else if (num_array[i] != 0) {
			printf("%u", num_array[i]);
			zero_skipped = true;
		}
	}
	printf("\n");
}

int main()
{
	int i, j;

	/* Get input two numbers */
	fgets(input1, MAX_NUMBER, stdin);
	fgets(input2, MAX_NUMBER, stdin);

	/* Clear add1 and add2 */
	memset(mul1, 0, sizeof(mul1));
	memset(mul2, 0, sizeof(mul2));
	memset(res, 0, sizeof(res));

	/* Convert string to int array */
	len1 = strlen(input1) - 1;
	for (i = 0; i < len1; i++)
		mul1[i] = input1[len1 - 1 - i] - '0';
	len2 = strlen(input2) - 1;
	for (i = 0; i < len2; i++)
		mul2[i] = input2[len2 - 1 - i] - '0';

	//print_num_array(mul1, len1);
	//print_num_array(mul2, len2);
	
	/* Multiply mul1 and mul2 manually */
	for (i = 0; i < len2; i++)
		for (j = 0; j < len1; j++)
			res[i + j] += mul2[i] * mul1[j];

	for (i = 0; i < (len1 + len2); i++) {
		res[i + 1] += res[i] / 10;
		res[i] %= 10;
	}

	print_num_array(res, len1 + len2);
	
	return 0;
}
