/*
 * =====================================================================================
 *
 *       Filename:  addition.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/06/2016 06:36:38 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NUMBER 220
char input1[MAX_NUMBER], input2[MAX_NUMBER];
int add1[MAX_NUMBER], add2[MAX_NUMBER];

void print_num_array(int num_array[MAX_NUMBER])
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
	printf("\n");
}

int main()
{
	int i, len;

	/* Get input two numbers */
	fgets(input1, MAX_NUMBER, stdin);
	fgets(input2, MAX_NUMBER, stdin);

	/* Clear add1 and add2 */
	memset(add1, 0, sizeof(add1));
	memset(add2, 0, sizeof(add2));

	/* Convert string to int array */
	len = strlen(input1) - 1;
	for (i = 0; i < len; i++)
		add1[i] = input1[len - 1 - i] - '0';
	len = strlen(input2) - 1;
	for (i = 0; i < len; i++)
		add2[i] = input2[len - 1 - i] - '0';

	//print_num_array(add1);
	//print_num_array(add2);

	/* Add add1 and add2 manually */
	for (i = 0; i < MAX_NUMBER - 10; i++) {
		add1[i] += add2[i];
		if (add1[i] >= 10) {
			add1[i] -= 10;
			add1[i + 1]++;
		}
	}

	printf("------------------------------\n");
	print_num_array(add1);

	return 0;
}
