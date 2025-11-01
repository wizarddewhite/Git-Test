#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

void read_mmap()
{
	char buffer[100];
	FILE * f = fopen("/proc/self/maps", "rb");

	if (f) {
		while (fgets(buffer, sizeof(buffer), f)) {
			printf("%s", buffer);
		}
		fclose(f);
	} else {
		printf("file open fail\n");
	}
}
 
int main(void) {
	size_t pagesize = 2 << 20;
	size_t mapsize = pagesize * 3;
	int unmap_result;

	printf("System page size: %lx bytes\n", pagesize);
	printf("### Before mmap:\n");
	read_mmap();

	getchar();

	/* Map a mapsize range with kernel chosen start address */
	char *region = mmap(
	  NULL,					/* kernel choose */
	  mapsize,				/* map size */
	  PROT_READ|PROT_WRITE|PROT_EXEC,	/* protection */
	  MAP_ANON|MAP_PRIVATE,			/* flags */
	  -1,
	  0
	);
	if (region == MAP_FAILED) {
		perror("Could not mmap");
		return 1;
	}

	printf("### After mmap: [%p - %p]\n", region, region + mapsize);
	read_mmap();

	getchar();

	// write some content to 2nd one
	strcpy(region + pagesize, "Hello, world!");
	*(region + 2 * pagesize - 1) = 'W';

	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	// unmap the 1st
	unmap_result = munmap(region, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return 1;
	}
	// unmap the 3rd
	unmap_result = munmap(region + 2 * pagesize, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return 1;
	}

	printf("### After munmap:\n");
	read_mmap();
	// show the content still there
	printf("Expect left region: [%p - %p]\n", region + pagesize, region + pagesize * 2);
	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	getchar();

	// move one page shift, can't overlap
	char *new_addr = region + (pagesize * 2) + (1 << 12);
	char *move_region = mremap(
			region + pagesize,		/* old_addr */
			pagesize,			/* old_size */
			pagesize,			/* new_size */
			MREMAP_FIXED | MREMAP_MAYMOVE,	/* flags */
			new_addr			/* new_addr */
		);
	if (move_region == MAP_FAILED) {
		perror("Could not remap");
		return 1;
	}

	if (move_region != new_addr) {
		printf("mremap failed to move to specified target\n");
		return 1;
	}

	printf("Move to region: [%p - %p]\n", move_region, move_region + pagesize);
	printf("Contents of region: %s %c\n",
		  move_region, *(move_region + pagesize - 1));

	getchar();

	return 0;
}
