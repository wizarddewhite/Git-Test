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
 
  char * region = mmap(
    NULL,
    mapsize,
    PROT_READ|PROT_WRITE|PROT_EXEC,
    MAP_ANON|MAP_PRIVATE,
    -1,
    0
  );
  if (region == MAP_FAILED) {
    perror("Could not mmap");
    return 1;
  } else {
    printf("Map to region: [%p - %p]\n", region, region + mapsize);
    printf("Exact  region: [%p - %p]\n", region + pagesize, region + pagesize * 2);
  }
 
  strcpy(region + pagesize, "Hello, world!");
 
  printf("Contents of region: %s\n", region + pagesize);
 
  unmap_result = munmap(region, pagesize);
  if (unmap_result) {
    perror("Could not munmap");
    return 1;
  }
  unmap_result = munmap(region + 2 * pagesize, pagesize);
  if (unmap_result) {
    perror("Could not munmap");
    return 1;
  }

  printf("Contents of region: %s\n", region + pagesize);
  printf("### After mmap:\n");
  read_mmap();
 
  getchar();

  return 0;
}
