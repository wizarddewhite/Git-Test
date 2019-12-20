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
    
    if (f)
    {
      while (fgets(buffer, sizeof(buffer), f)) {
        printf("%s", buffer);
      }
      fclose (f);
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
 
  // map a 3 times region
  char * region = mmap(
    NULL, // (void *)0x7f3d39000000
    mapsize,
    PROT_READ|PROT_WRITE|PROT_EXEC,
    MAP_ANON|MAP_PRIVATE,
    -1,
    0
  );
  if (region == MAP_FAILED) {
    perror("Could not mmap");
    return 1;
  }

  printf("Map to region: [%p - %p]\n", region, region + mapsize);
  printf("Exact  region: [%p - %p]\n", region + pagesize, region + pagesize * 2);
 
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

  // show the content still there
  printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));
  printf("### After mmap:\n");
  read_mmap();
 
  getchar();

  // move one page shift, can't overlap
  char *move_region = mremap(region + pagesize, pagesize, pagesize,
         MREMAP_FIXED | MREMAP_MAYMOVE, region + (pagesize * 2) + (1 << 12));
  if (move_region == MAP_FAILED) {
    perror("Could not remap");
    return 1;
  }

  printf("Move to region: [%p - %p]\n", move_region, move_region + pagesize);
  printf("Contents of region: %s %c\n",
		  move_region, *(move_region + pagesize - 1));

  getchar();

  return 0;
}
