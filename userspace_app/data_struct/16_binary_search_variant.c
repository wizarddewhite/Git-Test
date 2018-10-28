#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


int binary_search(int *arr, int size, int val)
{
	int low = 0, high = size - 1, mid;

	while (low <= high) {
		mid = (low + high) / 2;
		if (arr[mid] == val)
			return mid;

		if (arr[mid] < val)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return -1;
}

/* 
 * find the first index with *val*
 *
 * This is a little tricky because the calculation of mid is integer based, it
 * will be cast to the lower bound of an integer.
 * 
 * In case the [low, high] range is of size 1 or 2 and arr[mid] >= val, we will
 * have:
 *
 * mid = (low + high) / 2 = low
 * high = mid - 1 = low - 1 < low, which break the loop
 *
 */
int binary_search_first(int *arr, int size, int val)
{
	int low = 0, high = size - 1, mid;

	while (low <= high) {
		mid = (low + high) / 2;
		//printf("[%d-%d] %d\n", low, high, mid);

		if (arr[mid] >= val)
			high = mid - 1;
		else
			low = mid + 1;
	}

	//printf("[%d-%d] %d\n", low, high, mid);
	if (arr[low] == val)
		return low;
	else
		return -1;
}

int main()
{
	int arr[10] = {1, 4, 5, 9, 12, 14, 19, 19, 31, 36};
	int idx;

	idx = binary_search_first(arr, 10, 19);
	if (idx != -1)
		printf("first 19 at %d\n", idx);
	else
		printf("19 not in arr \n");

	return 0;
}
