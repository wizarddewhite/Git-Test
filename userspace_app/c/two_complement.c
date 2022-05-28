#include <stdio.h>
#include <stdlib.h>

/*
 * https://www.jianshu.com/p/f774a73a06ad
 *
 * For cpu with N bitwise word, it could represent [0, (2 ^ N) -1] whose
 * mod is 2 ^ N.
 *
 * Then we have
 *
 *   x =  mod + x
 *  -x =  mod + (-x)
 *
 * Because
 *
 *      x + (-x) = 0
 *   => x + (-x) = 0 + mod
 *   => x + (-x) = mod
 *
 * and
 *     0101  x
 *   + 1010 ~x
 *     ----
 *     1111
 *   +    1
 *     ----
 *    10000  mod
 *
 * which is
 *
 *     x + ~x + 1 = mod
 *
 * so
 *
 *   x + (-x) = x + ~x + 1
 *        -x  =     ~x + 1
 *
 */

void printByte(unsigned char byte)
{
	int idx;

	for (idx = 3; idx >= 0; idx--) {
		unsigned char mask = 1 << idx;

		if (byte & mask)
			printf("1");
		else
			printf("0");
	}
}

int main()
{
	int i;

	for (i = -8; i < 8; i++) {
		printf("%2d: ", i);
		printByte(i);
		printf("\n");
	}

	return 0;
}
