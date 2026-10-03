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

	munmap(hp, pmd_pagesize);
}

/*
 * Prepare hugetlbefs
 *
 * sudo mkdir -p /mnt/huge
 * sudo mount -t hugetlbfs -o pagesize=2M,size=128M,mode=0777 none /mnt/huge
 *
 */
void file_hugetlb()
{
	int fd;
	char *hp;
	unsigned long mem_size;
	char file_name[] = "/mnt/huge/demo";

	fd = open(file_name, O_CREAT | O_RDWR, 0600);
	if (fd < 0) {
		perror("open");
		return;
	}

	if (ftruncate(fd, pmd_pagesize) < 0) {
		perror("ftruncate");
		goto trunc_error;
	}

	hp = mmap(NULL, pmd_pagesize, PROT_READ | PROT_WRITE,
		  MAP_SHARED, fd, 0);
	if (hp == MAP_FAILED) {
		printf(RED "mmap failed: %s\n" RESET, strerror(errno));
		goto trunc_error;
	}

	/* should be 2M aligned */
	if ((uintptr_t)hp & (pmd_pagesize - 1)) {
		printf(RED "mmap unaligned to %lx\n",  (unsigned long)hp);
		goto mmap_error;
	}
	// printf("mapped at %p\n", hp);

	/* fault in */
	*hp = 'A';

	/* check the size is expected */
	mem_size = get_private_hugetlb(hp);
	if (mem_size == 2048)
		printf(GREEN "Private %lukb\n" RESET, mem_size);
	else
		printf(RED "hugetlb size is %lukb, but expect 204kb\n" RESET,
			mem_size);

mmap_error:
	munmap(hp, pmd_pagesize);
trunc_error:
	close(fd);
	unlink(file_name);
}

int main()
{
	pmd_pagesize = read_pmd_pagesize();
	if (!pmd_pagesize) {
		printf(RED "Reading PMD pagesize failed\n" RESET);
		exit(0);
	}

	// anon_hugetlb();
	file_hugetlb();
	return 0;
}
