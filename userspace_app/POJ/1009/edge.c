#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct rle {
	int val;
	int rep;
	int start_pos;
};

char line[1024];
struct rle pix[1024];
int width, total_pix;

#define DEBUG 0

#if DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT 
#endif

/* Return val of a pix in [1, total_pix]
 *
 * For a pos exceed the range, abort or return 0.
 */
int get_value(int pos)
{
	int i;
	if (pos < 0)
		abort();

	i = 0;
	for (i = 0;
	     pos > pix[i].rep && (pix[i].val || pix[i].rep);
	     i++)
		pos = pos - pix[i].rep;

	//printf("\tidx:%d\n", i);
	return pix[i].val;
}

void test_get_value(void)
{
	printf("Pos: %d, Value: %d\n", 4, get_value(4));
	printf("Pos: %d, Value: %d\n", 5, get_value(5));
	printf("Pos: %d, Value: %d\n", 27, get_value(27));
	printf("Pos: %d, Value: %d\n", 28, get_value(28));
	printf("Pos: %d, Value: %d\n", 34, get_value(34));
	printf("Pos: %d, Value: %d\n", 35, get_value(35));
	//printf("Pos: %d, Value: %d\n", 500000000, get_value(500000000));
	//printf("Pos: %d, Value: %d\n", 500000001, get_value(500000001));
}

int get_max_diff(int pos)
{
	int neighbour;
	int max_diff = 0, diff = 0;
	int self_val = get_value(pos);

	DEBUG_PRINT("##self val is %d\n", self_val);

	/* diff with left */
	if ((pos % width) != 1) {
		/* First pix of each row doesn't has this neighbour */

		/* upper left */
		neighbour = pos - width - 1;
		if (neighbour >= 1) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##upper left diff %d\n", diff);
		}

		/* left */
		neighbour = pos - 1;
		if (neighbour >= 1) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##left diff %d\n", diff);
		}

		/* bottom left */
		neighbour = pos + width - 1;
		if (neighbour <= total_pix) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##bottom left diff %d\n", diff);
		}
	}

	/* diff with upper */
	neighbour = pos - width;
	if (neighbour >= 1) {
		diff = abs(self_val - get_value(neighbour));
		if (diff > max_diff)
			max_diff = diff;
		DEBUG_PRINT("##upper diff %d\n", diff);
	}

	/* diff with bottom */
	neighbour = pos + width;
	if (neighbour <= total_pix) {
		diff = abs(self_val - get_value(neighbour));
		if (diff > max_diff)
			max_diff = diff;
		DEBUG_PRINT("##bottom diff %d\n", diff);
	}

	/* diff with right */
	if ((pos % width) != 0) {
		/* Last pix of each row doesn't has this neighbour */

		/* upper right */
		neighbour = pos - width + 1;
		if (neighbour >= 1) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##upper right diff %d\n", diff);
		}

		/* diff with right */
		neighbour = pos + 1;
		if (neighbour <= total_pix) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##right diff %d\n", diff);
		}


		/* diff with bottom right */
		neighbour = pos + width + 1;
		if (neighbour <= total_pix) {
			diff = abs(self_val - get_value(neighbour));
			if (diff > max_diff)
				max_diff = diff;
			DEBUG_PRINT("##bottom right diff %d\n", diff);
		}
	}

	return max_diff;
}

void test_get_max_diff()
{
	printf("Pos: %d, Max diff: %d\n", 1, get_max_diff(1));
	printf("Pos: %d, Max diff: %d\n", 4, get_max_diff(4));
	printf("Pos: %d, Max diff: %d\n", 7, get_max_diff(7));
	printf("Pos: %d, Max diff: %d\n", 21, get_max_diff(21));
	printf("Pos: %d, Max diff: %d\n", 22, get_max_diff(22));
	printf("Pos: %d, Max diff: %d\n", 25, get_max_diff(25));
	printf("Pos: %d, Max diff: %d\n", 35, get_max_diff(35));
}

void print_compress_image()
{
	int i;
	int diff_val, diff_val_old, len;
	//test_get_max_diff();
	printf("%d\n", width);
	
	diff_val = diff_val_old = 0;
	for (i = 1; i <= total_pix; i++) {
		diff_val = get_max_diff(i);

		if (i == 1) {
			diff_val_old = diff_val;
			len = 1;
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

void process_image()
{
	int idx;
	DEBUG_PRINT("Image width is %d\n", width);

	idx = 0;
	while(fgets(line, 1024, stdin)) {
		sscanf(line, "%d %d\n", &pix[idx].val, &pix[idx].rep);

		/* "0 0" means the end of an image */
		if (!pix[idx].val && !pix[idx].rep)
			break;

		if (!idx)
			pix[idx].start_pos = 0;
		else
			pix[idx].start_pos = pix[idx-1].start_pos + pix[idx-1].rep;

		idx++;
	}

	total_pix = pix[idx-1].start_pos + pix[idx-1].rep;
	DEBUG_PRINT("\tTotal pix is %d\n", total_pix);

	//test_get_value();
	print_compress_image();
	return;
}

int main()
{

	while(fgets(line, 1024, stdin)) {
		sscanf(line, "%d\n", &width);

		if (!width)
			break;
		process_image();
	}
	printf("0\n");
	
}
