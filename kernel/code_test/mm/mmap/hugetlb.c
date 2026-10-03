#include <fcntl.h>      // open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include <errno.h>
#include "vm_util.h"

uint64_t pmd_pagesize;

void anon_hugetlb()
{
	char *hp;
	unsigned long mem_size;

	hp = mmap(NULL, pmd_pagesize, PROT_READ | PROT_WRITE,
		  MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
	if (hp == MAP_FAILED) {
		printf(RED "mmap failed: %s\n" RESET, strerror(errno));
		return;
	}

	/* should be 2M aligned */
	if ((uintptr_t)hp & (pmd_pagesize - 1)) {
		printf(RED "mmap unaligned to %lx\n",  (unsigned long)hp);
		return;
	}

	/* fault in */
	*hp = 'A';
	/* check the size is expected */
	mem_size = get_private_hugetlb(hp);
	if (mem_size == 2048)
		printf(GREEN "Private_Hugetlb %lukb\n" RESET, mem_size);
	else
		printf(RED "hugetlb size is %lukb, but expect 204kb\n" RESET,
			mem_size);

	munmap(hp, 2 << 12);
}

int main()
{
	pmd_pagesize = read_pmd_pagesize();
	if (!pmd_pagesize) {
		printf(RED "Reading PMD pagesize failed\n" RESET);
		exit(0);
	}

	anon_hugetlb();
	return 0;
}
