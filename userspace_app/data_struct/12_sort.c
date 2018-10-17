#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void dump(int *arr, int size)
{
	int idx;

	for (idx = 0; idx < size; idx++)
		printf("%08d\n", arr[idx]);
}

void __merge(int *arr, int p, int q, int r)
{
	int *a, *b;
	int i, j, k;
	int size1, size2;

	size1 = q - p + 1;
	size2 = r - q;


	a = (int*)malloc(size1 * sizeof(int));
	b = (int*)malloc(size2 * sizeof(int));

	if (!a || !b)
		abort();

	memcpy(a, arr, size1 * sizeof(int));
	memcpy(b, arr + size1, size2 * sizeof(int));

	for (i = 0, j = 0, k = 0; i < size1 && j < size2;) {
		if (a[i] <= b[j])
			arr[k++] = a[i++];
		else
			arr[k++] = b[j++];
	}

	if (i == size1) {
		for (; j < size2;)
			arr[k++] = b[j++];
	} else {
		for (; i < size1;)
			arr[k++] = a[i++];
	}

	free(a);
	free(b);
}

void __merge_sort(int *arr, int p, int r)
{
	int q;

	if (p >= r)
		return;

	q = (p + r) / 2;
	__merge_sort(arr, p, q);
	__merge_sort(arr, q + 1, r);
	__merge(arr, p, q, r);
}

void merge_sort(int *arr, int size)
{
	__merge_sort(arr, 0, size - 1);
}

void merge_verify()
{
	int test[10] = {5, 8, 9, 23, 67, 1, 3, 7, 31, 56};

	__merge(test, 0, 4, 9);

	dump(test, 10);
}

void merge_sort_test()
{
	int test[10] = {5, 8, 9, 23, 67, 1, 3, 7, 31, 56};

	merge_sort(test, 10);

	dump(test, 10);
}

int main()
{
	//merge_verify();
	merge_sort_test();
	return 0;
}
