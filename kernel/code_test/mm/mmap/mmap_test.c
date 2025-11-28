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

void map_unmap_move()
{
	size_t pagesize = 1 << 12;
	size_t mapsize = pagesize * 3;
	int unmap_result;

	printf("System page size: %lx bytes\n", pagesize);
	printf("### Before mmap:\n");
	read_mmap();

	getchar();

	/* Map a 3 page range with kernel chosen start address */
	char *region = mmap(
	  NULL,					/* kernel choose */
	  mapsize,				/* map size */
	  PROT_READ|PROT_WRITE|PROT_EXEC,	/* protection */
	  MAP_ANONYMOUS|MAP_PRIVATE,		/* flags */
	  -1,
	  0
	);
	if (region == MAP_FAILED) {
		perror("Could not mmap");
		return;
	}

	printf("### After mmap: [%p - %p]\n", region, region + mapsize);
	read_mmap();

	getchar();

	// write some content to 2nd page
	strcpy(region + pagesize, "Hello, world!");
	*(region + 2 * pagesize - 1) = 'W';

	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	// unmap the 1st page
	unmap_result = munmap(region, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return;
	}
	// unmap the 3rd page
	unmap_result = munmap(region + 2 * pagesize, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return;
	}

	printf("### After munmap: [%p - %p] [%p - %p]\n",
		region, region + pagesize, region + 2 * pagesize, region + 3 * pagesize);
	read_mmap();
	// show the content still there
	printf("Expect left region: [%p - %p]\n", region + pagesize, region + pagesize * 2);
	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	getchar();

	// move one page shift, can't overlap
	char *new_addr = region + mapsize + pagesize;
	char *move_region = mremap(
			region + pagesize,		/* old_addr */
			pagesize,			/* old_size */
			pagesize,			/* new_size */
			MREMAP_FIXED | MREMAP_MAYMOVE,	/* flags */
			new_addr			/* new_addr */
		);
	if (move_region == MAP_FAILED) {
		perror("Could not remap");
		return;
	}

	if (move_region != new_addr) {
		printf("mremap failed to move to specified target\n");
		return;
	}

	printf("Move to region: [%p - %p] -> [%p - %p]\n", 
		region + pagesize, region + 2 * pagesize, move_region, move_region + pagesize);
	printf("Contents of moved region: %s %c\n",
		  move_region, *(move_region + pagesize - 1));
	read_mmap();

	getchar();

}
 
int main(void) {
	map_unmap_move();
	return 0;
}
