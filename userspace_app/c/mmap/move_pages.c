#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <numaif.h>

int main(void) {
  size_t pagesize = getpagesize();
  size_t mapsize = pagesize;
  void *pages[1];
  int nodes[1];
  int status[1];
  int ret;
 
  printf("System page size: %lx bytes\n", pagesize);
 
  // map a page region
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
 
  // write some content to 2nd one
  strcpy(region, "Hello, world!");
  printf("Contents of region: %s \n", region);
 
  pages[0] = region;
  // retrieve current numa info
  ret = move_pages(0, 1, (void **)&pages, NULL, status, MPOL_MF_MOVE);
  
  if (!ret) {
    printf("Current numa node is : %d\n", status[0]);
  }

  return 0;
}
