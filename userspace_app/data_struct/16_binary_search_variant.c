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

int binary_search_last(int *arr, int size, int val)
{
	int low = 0, high = size - 1, mid;

	while (low <= high) {
		mid = (low + high) / 2;

		if (arr[mid] <= val)
			low = mid + 1;
		else
			high = mid - 1;
	}

	if (arr[high] == val)
		return high;
	else
		return -1;
}

int binary_search_first_r(int *arr, int size, int val)
{
	int mid = size / 2;
	int idx;

	if (size <= 0)
		return -1;

	// we find *val* at mid, try first half
	if (arr[mid] == val) {
		idx = binary_search_first_r(arr, mid, val);
		return idx != -1 ? idx : mid;
	}

	// mid == 0 means size == 1
	// so the only element in array doesn't equal to val
	if (!mid)
		return -1;

	if (arr[mid] < val) {
		idx = binary_search_first_r(arr + mid + 1, size - mid - 1, val);
		if (idx != -1)
			idx += mid + 1;
	} else {
		idx = binary_search_first_r(arr, mid, val);
	}

	return idx;
}

int binary_search_last_r(int *arr, int size, int val)
{
	int mid = size / 2;
	int idx;

	if (size <= 0)
		return -1;

	// we find *val* at mid, try last half
	if (arr[mid] == val) {
		idx = binary_search_last_r(arr+mid+1, size-mid-1, val);
		if (idx != -1)
			mid += idx + 1;
		return mid;
	}

	// mid == 0 means size == 1
	// so the only element in array doesn't equal to val
	if (!mid)
		return -1;

	if (arr[mid] < val) {
		idx = binary_search_last_r(arr + mid + 1, size - mid - 1, val);
		if (idx != -1)
			idx += mid + 1;
	} else {
		idx = binary_search_last_r(arr, mid, val);
	}

	return idx;
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

	idx = binary_search_first_r(arr, 10, 19);
	if (idx != -1)
		printf("first 19 at %d\n", idx);
	else
		printf("19 not in arr \n");

	idx = binary_search_last(arr, 10, 19);
	if (idx != -1)
		printf("last 19 at %d\n", idx);
	else
		printf("19 not in arr \n");

	idx = binary_search_last_r(arr, 10, 19);
	if (idx != -1)
		printf("last 19 at %d\n", idx);
	else
		printf("19 not in arr \n");

	return 0;
}
