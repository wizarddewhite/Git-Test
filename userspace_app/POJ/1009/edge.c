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
unsigned long width, total_pix;

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

int get_max_diff(int pos)
{
	int neighbour;
	int max_diff = 0, diff = 0;
	int neighbour_val, self_val = get_value(pos);

	DEBUG_PRINT("##self val at pos(%d) is %d\n", pos, self_val);

	/* diff with left */
	if ((pos % width) != 1) {
		/* First pix of each row doesn't has this neighbour */

		/* upper left */
		neighbour = pos - width - 1;
		if (neighbour >= 1) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tupper left diff |%d - %d| = %d\n",
				    self_val, neighbour_val, diff);
		}

		/* left */
		neighbour = pos - 1;
		if (neighbour >= 1) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tleft diff |%d - %d| = %d\n",
				    self_val, neighbour_val, diff);
		}

		/* bottom left */
		neighbour = pos + width - 1;
		if (neighbour <= total_pix) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tbottom left diff |%d - %d| = %d\n",
				    self_val, neighbour_val, diff);
		}
	}

	/* diff with upper */
	neighbour = pos - width;
	if (neighbour >= 1) {
		neighbour_val = get_value(neighbour);
		diff = abs(self_val - neighbour_val);
		if (diff > max_diff)
			max_diff = diff;
		DEBUG_PRINT("\tupper diff |%d - %d| = %d\n",
			    self_val, neighbour_val, diff);
	}

	/* diff with bottom */
	neighbour = pos + width;
	if (neighbour <= total_pix) {
		neighbour_val = get_value(neighbour);
		diff = abs(self_val - neighbour_val);
		if (diff > max_diff)
			max_diff = diff;
		DEBUG_PRINT("\tbottom diff |%d - %d| = %d\n",
			    self_val, neighbour_val, diff);
	}

	/* diff with right */
	if ((pos % width) != 0) {
		/* Last pix of each row doesn't has this neighbour */

		/* upper right */
		neighbour = pos - width + 1;
		if (neighbour >= 1) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tupper right diff |%d - %d| = %d\n",
			            self_val, neighbour_val, diff);
		}

		/* diff with right */
		neighbour = pos + 1;
		if (neighbour <= total_pix) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tright diff |%d - %d| = %d\n",
			            self_val, neighbour_val, diff);
		}


		/* diff with bottom right */
		neighbour = pos + width + 1;
		if (neighbour <= total_pix) {
			neighbour_val = get_value(neighbour);
			diff = abs(self_val - neighbour_val);
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("\tbottom right diff |%d - %d| = %d\n",
			            self_val, neighbour_val, diff);
		}
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

void store_image()
{
	int idx = 0;
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
	DEBUG_PRINT("Total pix is %d\n", total_pix);
	//dump_image();

	//test_get_value();
	//test_get_max_diff();
	return;
}

void process_image()
{
	DEBUG_PRINT("Image width is %lu\n", width);
	store_image();

	print_compress_image();
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
