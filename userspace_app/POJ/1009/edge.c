#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT
#endif

struct rle {
	int val;
	unsigned long rep;
	unsigned long start_pos;
};

char line[1024];
struct rle pix[1024];
/* 1000 range at most,
 * each range has two end,
 * each end has 9 adj at most */
int boundary[1000 * 2 * 9];
int num_boundary;
unsigned long width, total_pix;

int adj[8][2] = {
	/* row, col */
	{-1, -1},
	{-1,  0},
	{-1,  1},
	{ 0, -1},
	{ 0,  1},
	{ 1, -1},
	{ 1,  0},
	{ 1,  1},
};

/* Return val of a pix in [1, total_pix]
 *
 * For a pos exceed the range, abort or return 0.
 */
int get_value(int pos)
{
	int i;
	if (pos <= 0)
		abort();

	DEBUG_PRINT("\tpos(%d) is in idx:", pos);
	i = 0;
	for (i = 0;
	     pos > pix[i].rep && (pix[i].val || pix[i].rep);
	     i++)
		pos = pos - pix[i].rep;

	DEBUG_PRINT("%d\n", i);
	return pix[i].val;
}

void test_get_value(void)
{
	DEBUG_PRINT("Pos: %d, Value: %d\n", 4, get_value(4));
	DEBUG_PRINT("Pos: %d, Value: %d\n", 5, get_value(5));
	DEBUG_PRINT("Pos: %d, Value: %d\n", 27, get_value(27));
	DEBUG_PRINT("Pos: %d, Value: %d\n", 28, get_value(28));
	DEBUG_PRINT("Pos: %d, Value: %d\n", 34, get_value(34));
	DEBUG_PRINT("Pos: %d, Value: %d\n", 35, get_value(35));
	//DEBUG_PRINT("Pos: %d, Value: %d\n", 500000000, get_value(500000000));
	//DEBUG_PRINT("Pos: %d, Value: %d\n", 500000001, get_value(500000001));
}

int is_valid_adj(int pos, int idx)
{
	int row, col;
	row = (pos - 1) / width;
	col = (pos - 1) % width;

	row = row + adj[idx][0];
	col = col + adj[idx][1];

	if (row == -1 || row == (total_pix / width))
		return 0;
	if (col == -1 || col == width)
		return 0;

	return 1;
}

void test_adj(int pos)
{
	int i;
	for (i = 0; i < 8; i++) {
		if (is_valid_adj(pos, i))
			printf("pos(%d)'s %d's adj is valid\n", pos, i);
	}
}

int get_max_diff(int pos)
{
	int neighbour;
	int max_diff = 0, diff = 0;
	int i;
	int neighbour_val, self_val = get_value(pos);
	DEBUG_PRINT("##self val at pos(%d) is %d\n", pos, self_val);

	for (i = 0; i < 8; i++) {
		if (!is_valid_adj(pos, i))
			continue;

		neighbour = pos + width * adj[i][0] + adj[i][1];
		neighbour_val = get_value(neighbour);
		diff = abs(self_val - neighbour_val);
		if (diff > max_diff)
			max_diff = diff;
	}

	DEBUG_PRINT("\tMax diff %d\n", max_diff);
	return max_diff;
}

void test_get_max_diff()
{
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 1, get_max_diff(1));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 4, get_max_diff(4));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 7, get_max_diff(7));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 21, get_max_diff(21));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 22, get_max_diff(22));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 25, get_max_diff(25));
	DEBUG_PRINT("Pos: %d, Max diff: %d\n", 35, get_max_diff(35));
}

int cal_skip(int pix_idx, unsigned long *skip_start, unsigned long *skip_len)
{
	/* Handle special case for a range with more than 3 * width len
	 *
	 * Like the pattern:
	 * AAAAAAAAAAAAAAAAA
	 * AAAAAAAAAAAAAAAAA
	 * AAAAAAAAAAAAAAAAA
	 *
	 * The middle one's max_diff are all 0.
	 * */
	if (pix[pix_idx].rep >= 3 * width) {
		*skip_start = pix[pix_idx].start_pos + width;
		*skip_len = pix[pix_idx].rep - 2 * width;
		return 0;
	}

	/* Handle special case for only one large line */
	if (pix_idx == 0 && !pix[1].val && !pix[1].rep
	    && !(pix[0].rep % width)) {
		*skip_start = 2;
		*skip_len = width - 3;
		return 0;
	}

	/* Handle special case for three line
	 *
	 * Like the pattern:
	 * AAAAAAAAAAAAAAAAA
	 * BBBBBBBBBBBBBBBBB
	 * CCCCCCCCCCCCCCCCC
	 *
	 * The middle line has the same diff value.
	 * */
	/* First we have two lines */
	if (pix[pix_idx].rep == width &&
	    pix[pix_idx+1].rep == width) {
		/* Maybe we have the same third line
		 * or this is end of the image*/
		if (pix[pix_idx+2].rep == width ||
		    pix[pix_idx+2].rep == 0) {
			*skip_start = pix[pix_idx].start_pos + width;
			*skip_len = width;
			return get_value(*skip_start);
		}
	}

	*skip_start = 0;
	*skip_len = 0;
	return 0;
}

void print_compress_image2()
{
	int i;
	int diff_val, diff_val_old;
	int index, index_old;

	index_old = 1;
	diff_val_old = get_max_diff(1);

	printf("%ld\n", width);

	for (i = 0; i < num_boundary; i++) {
		index = boundary[i];
		if (index == index_old)
			continue;

		diff_val = get_max_diff(index);
		if (diff_val != diff_val_old) {
			printf("%d %d\n", diff_val_old, index - index_old);
			index_old = index;
			diff_val_old = diff_val;
		}

	}
	printf("%d %lu\n", diff_val_old, total_pix - index_old + 1);
	printf("0 0\n");
}

void print_compress_image()
{
	int i;
	int diff_val, diff_val_old, len;
	bool skipped = true;
	int dup_val;  /* duplicated value */
	unsigned long skip_start, skip_len;
	int next_pix_pos, pix_idx;
	printf("%ld\n", width);
	
	next_pix_pos = 1;
	pix_idx = 0;
	diff_val = diff_val_old = -1;
	len = 0;
	for (i = 1; i <= total_pix; i++) {
		/* On each new pix range, we calculate the skip case */
		if (next_pix_pos == i) {
			dup_val = cal_skip(pix_idx, &skip_start, &skip_len);
			DEBUG_PRINT("\tpos(%d) skip_start(%ld) skip_len(%ld) dup_val(%d)\n",
				i, skip_start, skip_len, dup_val);

			/* move to next pix range */
			next_pix_pos = pix[pix_idx].start_pos + pix[pix_idx].rep;
			pix_idx++;
		}

		if (i == skip_start) {
			DEBUG_PRINT("\tat skip_start(%d) dup_val(%d) diff_val(%d) diff_val_old(%d)\n\tlen(%d) skip_len(%d)\n",
					i, dup_val, diff_val, diff_val_old, len, skip_len);
			i = i + skip_len - 1;
			skipped = true;

			/* We are skipped here with previous un-printed value */
			if (len) {
				/* the dup range has the same diff value as previous,
				 * so accumulate */
				if (dup_val == diff_val_old) {
					len += skip_len;
					DEBUG_PRINT("same val(%d) with len(%d)\n", dup_val, len);
				} else {
				/* otherwise, print what we have before */
					printf("%d %d\n", diff_val_old, len);
					len = skip_len;
					diff_val_old = dup_val;
				}
			}
			skip_start = skip_len = dup_val = 0;
			continue;
		}

		diff_val = get_max_diff(i);

		if (skipped) {
			/* The diff value of previous skip range has different
			 * value, just print it.
			 *
			 * Or accumulate the len.
			 * */
			if (i == 1) {
				diff_val_old = diff_val;
				len = 1;
			} else if (diff_val != diff_val_old) {
				printf("%d %d\n", diff_val_old, len);
				diff_val_old = diff_val;
				len = 1;
			} else {
				len++;
			}
			skipped = false;
			continue;
		}

		if (diff_val != diff_val_old) {
			printf("%d %d\n", diff_val_old, len);
			len = 1;
			diff_val_old = diff_val;
		} else {
			len++;
		}
	}
	printf("%d %d\n", diff_val, len);

	printf("0 0\n");
	return;
}

void dump_image()
{
	int idx;

	for (idx = 0; ;idx++) {
		printf("%d: (%d %lu) start_pos:%lu\n",
			    idx, pix[idx].val, pix[idx].rep,
			    pix[idx].start_pos);

		if (!pix[idx].val && !pix[idx].rep)
			break;
	}
	return;
}

int boundary_comp(const void *r1, const void *r2)
{
	int number1, number2;
	number1 = *(int *)r1; 
	number2 = *(int *)r2; 

	return number1 - number2;
}

void dump_boundary(void)
{
	int i;
	for (i = 0; i < num_boundary; i++)
		printf("%d %d\n", i, boundary[i]);
}

void store_image()
{
	int idx = 0;
	int a, i;

	num_boundary = 0;
	while(fgets(line, 1024, stdin)) {
		sscanf(line, "%d %ld\n", &pix[idx].val, &pix[idx].rep);

		/* "0 0" means the end of an image */
		if (!pix[idx].val && !pix[idx].rep)
			break;

		if (!idx)
			pix[idx].start_pos = 1;
		else
			pix[idx].start_pos = pix[idx-1].start_pos + pix[idx-1].rep;


		idx++;
	}

	total_pix = pix[idx-1].start_pos + pix[idx-1].rep - 1;
	DEBUG_PRINT("Total pix is %lu\n", total_pix);

	for (i = 0; i < idx; i++) {
		for (a = 0; a < 8; a++) {
			if (is_valid_adj(pix[i].start_pos, a)) {
				boundary[num_boundary++] = pix[i].start_pos +
					                 width * adj[a][0] + adj[a][1];
				DEBUG_PRINT("%lu boundary[%d] = %d\n", pix[i].start_pos, num_boundary-1, boundary[num_boundary-1]);
			}
		}
		boundary[num_boundary++] = pix[i].start_pos;
		DEBUG_PRINT("%lu boundary[%d] = %d\n", pix[i].start_pos, num_boundary-1, boundary[num_boundary-1]);
		for (a = 0; a < 8; a++) {
			if (is_valid_adj(pix[i].start_pos + pix[i].rep - 1, a)) {
				boundary[num_boundary++] = pix[i].start_pos + pix[i].rep - 1 +
					                 width * adj[a][0] + adj[a][1];
				DEBUG_PRINT("%lu boundary[%d] = %d\n", pix[i].start_pos + pix[i].rep - 1,
						num_boundary-1, boundary[num_boundary-1]);
			}
		}
		boundary[num_boundary++] = pix[i].start_pos + pix[i].rep - 1;
		DEBUG_PRINT("%lu boundary[%d] = %d\n", pix[i].start_pos + pix[i].rep - 1, num_boundary-1, boundary[num_boundary-1]);
	}
	boundary[num_boundary] = 0;

	qsort(boundary, num_boundary, sizeof(boundary[0]), boundary_comp);
	//dump_boundary();
	//dump_image();
	//test_adj(10);

	//test_get_value();
	//test_get_max_diff();
	return;
}

void process_image()
{
	DEBUG_PRINT("Image width is %lu\n", width);
	store_image();

	//print_compress_image();
	print_compress_image2();
	return;
}

int main()
{

	while(fgets(line, 1024, stdin)) {
		sscanf(line, "%ld\n", &width);

		if (!width)
			break;
		process_image();
	}
	printf("0\n");
	
}