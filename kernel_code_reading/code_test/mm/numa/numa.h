#ifndef NUMA_H
#define NUMA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  unsigned long long      __u64;
typedef  unsigned long long      u64;

typedef u64 phys_addr_t;
#define PAGE_SHIFT		12
#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)

#define NODES_SHIFT     10
#define MAX_NUMNODES    (1 << NODES_SHIFT)
#define NR_NODE_MEMBLKS		(MAX_NUMNODES*2)
#define	NUMA_NO_NODE	(-1)

#define ULLONG_MAX	(~0ULL)

struct numa_memblk {
	u64			start;
	u64			end;
	int			nid;
};

struct numa_meminfo {
	int			nr_blks;
	struct numa_memblk	blk[NR_NODE_MEMBLKS];
};

#define max(a,b) ((a) > (b) ? a : b)
#define min(a,b) ((a) < (b) ? a : b)

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern struct numa_meminfo numa_meminfo;
void dump_meminfo(struct numa_meminfo *mi);
int numa_add_memblk(int nid, u64 start, u64 end);
int numa_cleanup_meminfo(struct numa_meminfo *mi);
#endif // NUMA_H
