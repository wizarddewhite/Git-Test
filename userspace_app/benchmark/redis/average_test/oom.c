#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	char *fptr;
	long i, k;

	i = 50000000000L;

	do {
		fptr = (char *)malloc(i);
		if(fptr == NULL)
			i = i - 0x1000;
	} while (( fptr == NULL) && (i > 0));


	printf("allocated memory at %p with size %ld\n", fptr, i);

	/* for time to observe */
	sleep(15);
	/* so that the memory really gets allocated and not just reserved */
	for(k = 0; k < i; k += 0x1000) {
		fptr[k] = (char) (k & 255);
	}
	/* O.K. now you have 1 minute */
	sleep(60);
	/* clean up, if we get here */
	free(fptr);
	return(0);
}
