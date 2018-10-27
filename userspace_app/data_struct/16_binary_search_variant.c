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

int binary_search_first(int *arr, int size, int val)
{
	int low = 0, high = size - 1, mid;

	while (low <= high) {
		mid = (low + high) / 2;

		// got one *val*
		// search [low, mid-1] to find the first *val*
		if (arr[mid] == val) {
			int idx;
			for (idx = mid - 1; idx >= low; idx--) {
				if (arr[idx] != val)
					return idx+1;
			}
			// touch here means [low, mid-1] all equal *val*
			return low;
		}

		if (arr[mid] < val)
			low = mid + 1;
		else
			high = mid - 1;
	}

	return -1;
}

int main()
{
	int arr[10] = {1, 4, 5, 9, 12, 19, 19, 19, 31, 36};
	int idx;

	idx = binary_search_first(arr, 10, 19);
	if (idx != -1)
		printf("first 19 at %d\n", idx);
	else
		printf("19 not in arr \n");

	return 0;
}
