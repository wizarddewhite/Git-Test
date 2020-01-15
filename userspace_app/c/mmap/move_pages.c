#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <numaif.h>
#include <numa.h>

int main(void) {
  size_t pagesize = getpagesize();
  size_t mapsize = pagesize;
  void *pages[1];
  int nodes[1], cur_node, node = -1;
  int status[1];
  int ret;

  if (numa_available() < 0) {
    printf("Numa not available, Quit\n");
    exit(-1);
  }
 
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
    cur_node = status[0];
    printf("Current numa node is : %d\n", cur_node);
  }

  printf("Max numa node: %d\n", numa_max_node());
  // look for a numa node different from current one
  for (node = 0; node <= numa_max_node(); node++) {
      if (numa_bitmask_isbitset(numa_all_nodes_ptr, node) &&
          node != status[0]) {
          printf("Available numa node %d\n", node);
          break;
      }
  }

  if (node > numa_max_node()) {
      printf("Couldn't find available numa node for testing\n");
      exit(-1);
  }

  printf("Move page %p from Node %d to Node %d\n", region, cur_node, node);
  nodes[0] = node;
  ret = move_pages(0, 1, (void **)&pages, nodes, status, MPOL_MF_MOVE);

  return 0;
}
