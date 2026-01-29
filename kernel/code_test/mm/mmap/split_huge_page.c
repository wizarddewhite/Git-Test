#define _GNU_SOURCE
#include <fcntl.h>      // open()
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "vm_util.h"

#define SPLIT_DEBUGFS "/sys/kernel/debug/split_huge_pages"
#define SMAP_PATH "/proc/self/smaps"
#define INPUT_MAX 80

#define PID_FMT "%d,0x%lx,0x%lx,%d"
#define PID_FMT_OFFSET "%d,0x%lx,0x%lx,%d,%d"
#define PATH_FMT "%s,0x%lx,0x%lx,%d"

uint64_t pagesize;
unsigned int pageshift;
uint64_t pmd_pagesize;
unsigned int pmd_order;
unsigned int pmd_order;
int *expected_orders;

const char *pagemap_proc = "/proc/self/pagemap";
const char *kpageflags_proc = "/proc/kpageflags";
int pagemap_fd;
int kpageflags_fd;

static void write_file(const char *path, const char *buf, size_t buflen)
{
	int fd;
	ssize_t numwritten;

	fd = open(path, O_WRONLY);
	if (fd == -1) {
		printf("%s open failed: %s\n", path, strerror(errno));
		exit(0);
	}

	numwritten = write(fd, buf, buflen - 1);
	close(fd);
	if (numwritten < 1) {
		printf("Write failed\n");
		exit(0);
	}
}

static void write_debugfs(const char *fmt, ...)
{
	char input[INPUT_MAX];
	int ret;
	va_list argp;

	va_start(argp, fmt);
	ret = vsnprintf(input, INPUT_MAX, fmt, argp);
	va_end(argp);

	if (ret >= INPUT_MAX) {
		printf("%s: Debugfs input is too long\n", __func__);
		exit(0);
	}

	write_file(SPLIT_DEBUGFS, input, ret + 1);
}

void init(void)
{
	if (geteuid() != 0) {
		printf("Please run the benchmark as root\n");
		exit(0);
	}

	pagesize = getpagesize();
	pageshift = ffs(pagesize) - 1;
	pmd_pagesize = read_pmd_pagesize();
	if (!pmd_pagesize) {
		printf("Reading PMD pagesize failed\n");
		exit(0);
	}

	pmd_order = sz2ord(pmd_pagesize, pagesize);

	pagemap_fd = open(pagemap_proc, O_RDONLY);
	if (pagemap_fd == -1) {
		printf("read pagemap: %s\n", strerror(errno));
		exit(0);
	}

	kpageflags_fd = open(kpageflags_proc, O_RDONLY);
	if (kpageflags_fd == -1) {
		printf("read kpageflags: %s\n", strerror(errno));
		exit(0);
	}
}

void split_huge_anon_page(void)
{
	char *one_page;
	int nr_pmds = 1;
	size_t len = nr_pmds * pmd_pagesize;
	size_t i;

	one_page = memalign(pmd_pagesize, len);
	if (!one_page) {
		printf("Fail to allocate memory: %s\n", strerror(errno));
		return;
	}

	madvise(one_page, len, MADV_HUGEPAGE);

	// fault in to alloc huge page and for further data check
	for (i = 0; i < len; i++)
		one_page[i] = (char)i;

	if (!check_huge_anon(one_page, nr_pmds, pmd_pagesize)) {
		printf("No THP is allocated\n");
		return;
	}
	is_addr_thp("\t", one_page, kpageflags_fd);
	show_vma_anon_stat("expect huge:", one_page);

	write_debugfs(PID_FMT, getpid(), (uint64_t)one_page,
		(uint64_t)one_page + len, 0);

	// check data integrity
	for (i = 0; i < len; i++) {
		if (one_page[i] != (char)i) {
			printf("%ld byte corrupted\n", i);
			return;
		}
	}

	is_addr_thp("\t", one_page, kpageflags_fd);
	show_vma_anon_stat("expect no huge:", one_page);

	free(one_page);

	return;
}

// let fork 3 process and do split in grand child
void split_multi_mapped_huge_anon_page()
{
	char *one_page;
	int nr_pmds = 1;
	size_t len = nr_pmds * pmd_pagesize;
	size_t i;
	pid_t pid;

	one_page = memalign(pmd_pagesize, len);
	if (!one_page) {
		printf("Fail to allocate memory: %s\n", strerror(errno));
		return;
	}

	madvise(one_page, len, MADV_HUGEPAGE);

	// fault in to alloc huge page and for further data check
	for (i = 0; i < len; i++)
		one_page[i] = (char)i;

	if (!check_huge_anon(one_page, nr_pmds, pmd_pagesize)) {
		printf("No THP is allocated\n");
		return;
	}
	is_addr_thp("\t", one_page, kpageflags_fd);
	show_vma_anon_stat("expect huge:", one_page);

	pid = fork();
	if (pid < 0) {
		perror("fork child\n");
	} else if (pid == 0) {
		printf("..in child %d\n", getpid());

		pid = fork();
		if (pid < 0) {
			perror("fork grand child\n");
		} else if (pid == 0) {
			printf("..in grand child %d\n", getpid());

			printf("Before split...\n");
			is_addr_thp("\t", one_page, kpageflags_fd);
			show_vma_anon_stat("expect huge:", one_page);

			write_debugfs(PID_FMT, getpid(), (uint64_t)one_page,
				(uint64_t)one_page + len, 0);

			printf("After split...\n");
			// check data integrity
			for (i = 0; i < len; i++) {
				if (one_page[i] != (char)i) {
					printf("%ld byte corrupted\n", i);
					return;
				}
			}

			is_addr_thp("\t", one_page, kpageflags_fd);
			show_vma_anon_stat("expect no huge:", one_page);

			free(one_page);
			sleep(5);
		} else {
			wait(NULL);
			printf("===grand child quit\n");
			is_addr_thp("\t", one_page, kpageflags_fd);
			show_vma_anon_stat("expect no huge:", one_page);
		}
	} else {
		wait(NULL);
		printf("===child quit\n");
		is_addr_thp("\t", one_page, kpageflags_fd);
		show_vma_anon_stat("expect no huge:", one_page);
	}
}

int main(void)
{
	// don't move around this
	init();

	// split_huge_anon_page();
	split_multi_mapped_huge_anon_page();

	return 0;
}
