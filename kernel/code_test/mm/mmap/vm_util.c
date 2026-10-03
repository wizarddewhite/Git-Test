/*
 * Copy from linux/tools/testing/selftest/mm/vm_util.c
 *
 */
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/userfaultfd.h>
#include <linux/fs.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include "vm_util.h"

#define MAX_LINE_LENGTH 500

/*
 * Read fp to buf, until it find pattern.
 */
static bool check_for_pattern(FILE *fp, const char *pattern, char *buf, size_t len)
{
	while (fgets(buf, len, fp)) {
		if (!strncmp(buf, pattern, strlen(pattern)))
			return true;
	}
	return false;
}

uint64_t read_pmd_pagesize(void)
{
	int fd;
	char buf[20];
	ssize_t num_read;

	fd = open("/sys/kernel/mm/transparent_hugepage/hpage_pmd_size", O_RDONLY);
	if (fd == -1)
		return 0;

	num_read = read(fd, buf, 19);
	if (num_read < 1) {
		close(fd);
		return 0;
	}
	buf[num_read] = '\0';
	close(fd);

	return strtoul(buf, NULL, 10);
}

/*
 * /proc/self/pagemap -- vaddr based file.
 *
 * Each vaddr has an entry.
 * Each entry contain its pfn, present bit, swap type, etc.
 *
 * @addr: the virtual address to query
 * Return the raw data of the entry at @addr
 */
uint64_t pagemap_get_entry(char *addr)
{
	const unsigned long pfn = (unsigned long)addr / getpagesize();
	uint64_t entry;
	int ret;
	int pagemap_fd;

	pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
	if (pagemap_fd == -1)
		return 0;

	ret = pread(pagemap_fd, &entry, sizeof(entry), pfn * sizeof(entry));
	close(pagemap_fd);
	if (ret != sizeof(entry))
		printf("reading pagemap failed\n");
	return entry;
}

unsigned long pagemap_get_pfn(char *addr)
{
	uint64_t entry = pagemap_get_entry(addr);

	/* If present (63th bit), PFN is at bit 0 -- 54. */
	if (entry & PM_PRESENT)
		return entry & 0x007fffffffffffffull;
	return -1ul;
}

void pagemap_get_info(struct pagemap_info *info)
{
	uint64_t entry = pagemap_get_entry(info->addr);

	/* If present (63th bit), PFN is at bit 0 -- 54. */
	if (entry & PM_PRESENT)
		info->pfn = entry & 0x007fffffffffffffull;

	info->is_file = !!(entry & PM_FILE);
}

/*
 * /proc/self/smaps -- vma based file
 *
 * Each vma has an entry. Each entry contains its status.
 *
 * 7f1234000000-7f1234001000 rw-p 00000000 00:00 0          [heap]
 * Size:                  4 kB
 * Rss:                   4 kB
 * Pss:                   4 kB
 * Shared_Clean:          0 kB
 * Shared_Dirty:          0 kB
 * Private_Clean:         0 kB
 * Private_Dirty:         4 kB
 * Referenced:            4 kB
 * Anonymous:             4 kB
 * LazyFree:              0 kB
 * AnonHugePages:         0 kB
 * ShmemPmdMapped:        0 kB
 * FilePmdMapped:         0 kB
 * Shared_Hugetlb:        0 kB
 * Private_Hugetlb:       0 kB
 * Swap:                  0 kB
 * SwapPss:               0 kB
 * Locked:                0 kB
 * THPeligible:           0
 * VmFlags: rd wr mr mw me ac sd
 *
 * @addr: virtual address in query, must equals to the vma->vm_start
 * @pattern: data to match, e.g. AnonHugePages
 * @buf, @len: specify a buffer to read data
 *
 * This function search /proc/self/smaps for a vma whose vm_start is @addr.
 * Then it looks for @pattern for it.
 *
 * Return pointer into buf if found pattern, NULL if not.
 */
char *__get_smap_entry(void *addr, const char *pattern, char *buf, size_t len)
{
	int ret;
	FILE *fp;
	char *entry = NULL;
	char addr_pattern[MAX_LINE_LENGTH];

	ret = snprintf(addr_pattern, MAX_LINE_LENGTH, "%08lx-",
		       (unsigned long) addr);
	if (ret >= MAX_LINE_LENGTH)
		exit(-1);

	fp = fopen("/proc/self/smaps", "r");
	if (!fp)
		exit(-1);

	if (!check_for_pattern(fp, addr_pattern, buf, len))
		goto err_out;

	/* Fetch the pattern in the same block */
	if (!check_for_pattern(fp, pattern, buf, len))
		goto err_out;

	/* Trim trailing newline */
	entry = strchr(buf, '\n');
	if (entry)
		*entry = '\0';

	entry = buf + strlen(pattern);

err_out:
	fclose(fp);
	return entry;
}

/*
 * Check in range start at @addr
 *
 * @addr: vma->vm_start
 * @pattern: status in smaps
 *
 * Check if the status specified by @pattern equals to (nr_hpages * hpage_size)
 */
bool __check_range(void *addr, char *pattern, int nr_hpages,
		  uint64_t hpage_size)
{
	char buffer[MAX_LINE_LENGTH];
	uint64_t val = -1;
	char *entry;

	entry = __get_smap_entry(addr, pattern, buffer, sizeof(buffer));
	if (!entry)
		goto err_out;

	if (sscanf(entry, "%9" SCNu64 " kB", &val) != 1)
		exit(-1);

err_out:
	return val == (nr_hpages * (hpage_size >> 10));
}

bool check_huge_anon(void *addr, int nr_hpages, uint64_t hpage_size)
{
	return __check_range(addr, "AnonHugePages: ", nr_hpages, hpage_size);
}

bool check_anon(void *addr, int nr_hpages, uint64_t page_size)
{
	return __check_range(addr, "Anonymous: ", nr_hpages, page_size);
}

/*
 * Return the value specified by @pattern in vma start at @addr.
 *
 * @addr: vma->vm_start
 * @pattern: status in smaps
 */
uint64_t __get_range(void *addr, char *pattern)
{
	char buffer[MAX_LINE_LENGTH];
	uint64_t val = 0;
	char *entry;

	entry = __get_smap_entry(addr, pattern, buffer, sizeof(buffer));
	if (!entry)
		goto err_out;

	if (sscanf(entry, "%9" SCNu64 " kB", &val) != 1)
		exit(-1);

err_out:
	return val;
}

uint64_t get_huge_anon(void *addr)
{
	return __get_range(addr, "AnonHugePages: ");
}

uint64_t get_anon(void *addr)
{
	return __get_range(addr, "Anonymous: ");
}

uint64_t get_private_hugetlb(void *addr)
{
	return __get_range(addr, "Private_Hugetlb: ");
}

uint64_t get_shared_hugetlb(void *addr)
{
	return __get_range(addr, "Shared_Hugetlb: ");
}

uint64_t get_shmem_pmd_mapped(void *addr)
{
	return __get_range(addr, "ShmemPmdMapped: ");
}

void show_vma_anon_stat(char *prefix, void *addr)
{
	printf("\t%s huge anon %lukb, anon %lukb\n", prefix,
		get_huge_anon(addr), get_anon(addr));
}

/*
 * /proc/kpageflags -- pfn based file.
 *
 * Each pfn has an entry, specifying page status, e.g. Dirty, LRU, Buddy,
 * anon, etc.
 *
 * @pfn: the pfn to query
 * @flags: pfn's flags
 */
int pageflags_get(unsigned long pfn, uint64_t *flags)
{
	size_t count;
	int fd;

	fd = open("/proc/kpageflags", O_RDONLY);
	if (fd == -1)
		return -1;

	count = pread(fd, flags, sizeof(*flags),
		      pfn * sizeof(*flags));
	close(fd);

	if (count != sizeof(*flags))
		return -1;

	return 0;
}

/*
 * get pageflags of a @vaddr
 *
 * @vaddr: virtual address to query
 * @flags: page flag of the pfn mapped
 */
int vaddr_pageflags_get(char *vaddr, uint64_t *flags)
{
	unsigned long pfn;

	pfn = pagemap_get_pfn(vaddr);

	/* non-present PFN */
	if (pfn == -1UL)
		return 1;

	if (pageflags_get(pfn, flags))
		return -1;

	return 0;
}

/*
 * Check if @addr is backed by THP.
 *
 * @prefix: for output
 * @addr: the virtual add to query
 */
void is_addr_thp(char *prefix, char *addr)
{
	const uint64_t folio_head_flags = KPF_THP | KPF_COMPOUND_HEAD;
	const uint64_t folio_tail_flags = KPF_THP | KPF_COMPOUND_TAIL;
	unsigned long pfn;
	uint64_t pfn_flags;

	pfn = pagemap_get_pfn(addr);
	pageflags_get(pfn, &pfn_flags);

	if (!(pfn_flags & KPF_THP)) {
		printf("%svaddr(%lx) at pfn(%lx) isn't THP\n",
			prefix, (unsigned long)addr, pfn);
		return;
	}

	if ((pfn_flags & folio_head_flags) == folio_head_flags)
		printf("%svaddr(%lx) at pfn(%lx) is Head\n",
			prefix, (unsigned long)addr, pfn);
	else if ((pfn_flags & folio_tail_flags) == folio_tail_flags)
		printf("%svaddr(%lx) at pfn(%lx) is Tail\n",
			prefix, (unsigned long)addr, pfn);
	else // not expected
		printf("%svaddr(%lx) at pfn(%lx) is UNKNOWN\n",
			prefix, (unsigned long)addr, pfn);
}

/*
 * get page number mapped in @addr_start
 *
 * @addr_start: virtual address to query
 *
 * As @addr_start may not be head, the number of pages nay not equal to
 * folio_nr_pages().
 *
 * Return number of pages, 0 means no pages mapped.
 */
int vaddr_page_number(char *addr_start, uint64_t pagesize)
{
	char *addr;
	uint64_t page_flags;
	bool is_head = false, start_is_head = false;
	int nr_pages = 0;

	for (addr = addr_start; ; addr += pagesize) {
		int status;

		status = vaddr_pageflags_get(addr, &page_flags);
		if (status < 0)
			break;

		/* skip non-present addr */
		if (status == 1)
			break;

		nr_pages++;

		/* this is not a thp */
		if (!(page_flags & KPF_THP))
			break;

		/* one of head/tail must set */
		if (!(page_flags & (KPF_COMPOUND_HEAD | KPF_COMPOUND_TAIL)))
			break;

		/* we meet a thp at least. */
		is_head = page_flags & KPF_COMPOUND_HEAD;
		if (addr == addr_start)
			start_is_head = is_head;
		else if (is_head) // stop if we meet a new head
			break;
	}

	if (is_head)
		nr_pages--;
	return nr_pages;
}

/*
 * /proc/kpagecount -- pfn based file.
 *
 * Each pfn has an entry, maintain the mapcount.
 *
 * @pfn: the pfn to query
 * @mapcount: pfn's mapcount
 *
 * Return 0 on success, -1 otherwise.
 */
int pagemapcount_get(unsigned long pfn, uint64_t *mapcount)
{
	size_t count;
	int kpageflags_fd;

	kpageflags_fd = open("/proc/kpagecount", O_RDONLY);
	if (kpageflags_fd == -1) {
		printf("read kpagecount: %s\n", strerror(errno));
		return -1;
	}

	count = pread(kpageflags_fd, mapcount, sizeof(*mapcount),
		      pfn * sizeof(*mapcount));

	if (count != sizeof(*mapcount))
		return -1;

	return 0;
}
