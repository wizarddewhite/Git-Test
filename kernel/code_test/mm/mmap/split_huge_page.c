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

static int vaddr_pageflags_get(char *vaddr, int pagemap_fd, int kpageflags_fd,
		uint64_t *flags)
{
	unsigned long pfn;

	pfn = pagemap_get_pfn(vaddr);

	/* non-present PFN */
	if (pfn == -1UL)
		return 1;

	if (pageflags_get(pfn, kpageflags_fd, flags))
		return -1;

	return 0;
}

/*
 * gather_after_split_folio_orders - scan through [vaddr_start, len) and record
 * folio orders
 *
 * @vaddr_start: start vaddr
 * @len: range length
 * @pagemap_fd: file descriptor to /proc/<pid>/pagemap
 * @kpageflags_fd: file descriptor to /proc/kpageflags
 * @orders: output folio order array
 * @nr_orders: folio order array size
 *
 * gather_after_split_folio_orders() scan through [vaddr_start, len) and check
 * all folios within the range and record their orders. All order-0 pages will
 * be recorded. Non-present vaddr is skipped.
 *
 * NOTE: the function is used to check folio orders after a split is performed,
 * so it assumes [vaddr_start, len) fully maps to after-split folios within that
 * range.
 *
 * Return: 0 - no error, -1 - unhandled cases
 */
static int gather_after_split_folio_orders(char *vaddr_start, size_t len,
		int pagemap_fd, int kpageflags_fd, int orders[], int nr_orders)
{
	uint64_t page_flags = 0;
	int cur_order = -1;
	char *vaddr;

	if (pagemap_fd == -1 || kpageflags_fd == -1)
		return -1;
	if (!orders)
		return -1;
	if (nr_orders <= 0)
		return -1;

	for (vaddr = vaddr_start; vaddr < vaddr_start + len;) {
		char *next_folio_vaddr;
		int status;

		status = vaddr_pageflags_get(vaddr, pagemap_fd, kpageflags_fd,
					&page_flags);
		if (status < 0)
			return -1;

		/* skip non present vaddr */
		if (status == 1) {
			vaddr += pagesize;
			continue;
		}

		/* all order-0 pages with possible false postive (non folio) */
		if (!(page_flags & (KPF_COMPOUND_HEAD | KPF_COMPOUND_TAIL))) {
			orders[0]++;
			vaddr += pagesize;
			continue;
		}

		/* skip non thp compound pages */
		if (!(page_flags & KPF_THP)) {
			vaddr += pagesize;
			continue;
		}

		/* vpn points to part of a THP at this point */
		if (page_flags & KPF_COMPOUND_HEAD)
			cur_order = 1;
		else {
			vaddr += pagesize;
			continue;
		}

		next_folio_vaddr = vaddr + (1UL << (cur_order + pageshift));

		if (next_folio_vaddr >= vaddr_start + len)
			break;

		while ((status = vaddr_pageflags_get(next_folio_vaddr,
						     pagemap_fd, kpageflags_fd,
						     &page_flags)) >= 0) {
			/*
			 * non present vaddr, next compound head page, or
			 * order-0 page
			 */
			if (status == 1 ||
			    (page_flags & KPF_COMPOUND_HEAD) ||
			    !(page_flags & (KPF_COMPOUND_HEAD | KPF_COMPOUND_TAIL))) {
				if (cur_order < nr_orders) {
					orders[cur_order]++;
					cur_order = -1;
					vaddr = next_folio_vaddr;
				}
				break;
			}

			cur_order++;
			next_folio_vaddr = vaddr + (1UL << (cur_order + pageshift));
		}

		if (status < 0)
			return status;
	}
	if (cur_order > 0 && cur_order < nr_orders)
		orders[cur_order]++;
	return 0;
}

static int check_after_split_folio_orders(char *vaddr_start, size_t len,
		int pagemap_fd, int kpageflags_fd, int orders[], int nr_orders)
{
	int *vaddr_orders;
	int status;
	int i;

	vaddr_orders = (int *)malloc(sizeof(int) * nr_orders);

	if (!vaddr_orders) {
		perror("Cannot allocate memory for vaddr_orders");
		return -1;
	}

	memset(vaddr_orders, 0, sizeof(int) * nr_orders);
	status = gather_after_split_folio_orders(vaddr_start, len, pagemap_fd,
				     kpageflags_fd, vaddr_orders, nr_orders);
	if (status)
		printf("gather folio info failed\n");

	for (i = 0; i < nr_orders; i++)
		if (vaddr_orders[i] != orders[i]) {
			printf("order %d: expected: %d got %d\n", i,
				       orders[i], vaddr_orders[i]);
			status = -1;
		}

	free(vaddr_orders);
	return status;
}

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

	expected_orders = (int *)malloc(sizeof(int) * (pmd_order + 1));
	if (!expected_orders) {
		printf("Fail to allocate memory: %s\n", strerror(errno));
		exit(0);
	}

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
	unsigned long pfn;
	uint64_t mapcount;

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

	pfn = pagemap_get_pfn(one_page);
	if (pfn == -1ul) {
		printf("Failed to get pfn of the range\n");
		return;
	}
	if (!pagemapcount_get(pfn, &mapcount))
		printf("\tmapcount of before split: %lu\n", mapcount);
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

	if (!pagemapcount_get(pfn, &mapcount))
		printf("\tmapcount of after split: %lu\n", mapcount);
	is_addr_thp("\t", one_page, kpageflags_fd);
	show_vma_anon_stat("expect no huge:", one_page);

	free(one_page);

	return;
}

// let fork number process and do split in grand child
void split_multi_mapped_huge_anon_page()
{
	char *one_page;
	int nr_pmds = 1;
	size_t len = nr_pmds * pmd_pagesize;
	size_t i;
	pid_t pid;
	int level = 0;

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

	for (;;) {
		pid = fork();

		if (pid < 0) {
			perror("fork \n");
			exit(1);
		}

		if (pid == 0) {
			level++;
			// printf("child %d %02d\n", getpid(), level);

			// determine the number of child
			if (level == 5) {
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

				memset(expected_orders, 0, sizeof(int) * (pmd_order + 1));
				expected_orders[0] = 1 << pmd_order;

				if (check_after_split_folio_orders(one_page, len, pagemap_fd,
								   kpageflags_fd, expected_orders,
								   (pmd_order + 1)))
					printf("!!!Unexpected THP split\n");
				else
					printf("###Folio split to order 0\n");

				free(one_page);
				exit(0);
			}
		} else {
			// parent break the loop
			break;
		}
	}

	// wait for child
	wait(NULL);

	// printf("===child quit\n");
	// is_addr_thp("\t", one_page, kpageflags_fd);
	// show_vma_anon_stat("expect no huge:", one_page);
	free(one_page);

	if (level != 0)
		exit(0);
}

int main(void)
{
	// don't move around this
	init();

	// split_huge_anon_page();
	split_multi_mapped_huge_anon_page();
	printf("test continue %d\n",  getpid());

	return 0;
}
