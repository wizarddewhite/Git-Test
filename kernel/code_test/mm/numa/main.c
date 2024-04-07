#include "numa.h"

void numa_meminfo_normal1()
{
	/*
	 * Node 0           Node 1            Node 2          Node 3           
	 * [0x1000, 0x1FFF] [0x2000, 0x2FFF] [0x3000, 0x3FFF] [0x4000, 0x4FFF]
	 */
	numa_add_memblk(0, 0x1000, 0x1FFF + 1);
	numa_add_memblk(2, 0x3000, 0x3FFF + 1);
	numa_add_memblk(3, 0x4000, 0x4FFF + 1);
	numa_add_memblk(1, 0x2000, 0x2FFF + 1);

	numa_cleanup_meminfo(&numa_meminfo);
	dump_meminfo(&numa_meminfo);
}

void numa_meminfo_normal2()
{
	/*
	 * Node 0           Node 1            Node 2          Node 3           
	 * [0x1000, 0x1FFF] [0x2000, 0x2FFF] [0x3000, 0x3FFF] [0x4000, 0x4FFF]
	 */
	/* Neighbour */
	numa_add_memblk(0, 0x1000, 0x14FF + 1);
	numa_add_memblk(0, 0x1500, 0x1FFF + 1);

	/* Overlapping */
	numa_add_memblk(2, 0x3000, 0x34FF + 1);
	numa_add_memblk(2, 0x3300, 0x3FFF + 1);

	numa_add_memblk(3, 0x4000, 0x4FFF + 1);
	numa_add_memblk(1, 0x2000, 0x2FFF + 1);

	numa_cleanup_meminfo(&numa_meminfo);
	dump_meminfo(&numa_meminfo);
}

void numa_meminfo_abnormal()
{
	/*
	 * Node 0           Node 1            Node 2          Node 3           Node 0           
	 * [0x1000, 0x1FFF] [0x2000, 0x2FFF] [0x3000, 0x3FFF] [0x4000, 0x4FFF] [0x5000, 0x5FFF]
	 */
	/* Jump -- First part */
	numa_add_memblk(0, 0x1000, 0x1FFF + 1);

	/* Overlapping */
	numa_add_memblk(2, 0x3000, 0x34FF + 1);
	numa_add_memblk(2, 0x3300, 0x3FFF + 1);

	numa_add_memblk(3, 0x4000, 0x4FFF + 1);
	numa_add_memblk(1, 0x2000, 0x2FFF + 1);

	/* Jump -- Second part */
	numa_add_memblk(0, 0x5000, 0x5FFF + 1);

	numa_cleanup_meminfo(&numa_meminfo);
	dump_meminfo(&numa_meminfo);
}

int main()
{
	numa_meminfo_abnormal();
	return 0;
}

