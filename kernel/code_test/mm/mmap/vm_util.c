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

#define PMD_SIZE_FILE_PATH "/sys/kernel/mm/transparent_hugepage/hpage_pmd_size"
#define SMAP_FILE_PATH "/proc/self/smaps"
#define KPAGECOUNT_FILE_PATH "/proc/kpagecount"
#define MAX_LINE_LENGTH 500

uint64_t pagemap_get_entry(char *start)
{
	const unsigned long pfn = (unsigned long)start / getpagesize();
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

unsigned long pagemap_get_pfn(char *start)
{
	uint64_t entry = pagemap_get_entry(start);

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

uint64_t read_pmd_pagesize(void)
{
	int fd;
	char buf[20];
	ssize_t num_read;

	fd = open(PMD_SIZE_FILE_PATH, O_RDONLY);
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

bool check_for_pattern(FILE *fp, const char *pattern, char *buf, size_t len)
{
	while (fgets(buf, len, fp)) {
		if (!strncmp(buf, pattern, strlen(pattern)))
			return true;
	}
	return false;
}

/*
 * addr must equals to the vma->vm_start
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

	fp = fopen(SMAP_FILE_PATH, "r");
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

void show_vma_anon_stat(char *prefix, void *addr)
{
	printf("\t%s huge anon %lukb, anon %lukb\n", prefix,
		get_huge_anon(addr), get_anon(addr));
}

int pageflags_get(unsigned long pfn, int kpageflags_fd, uint64_t *flags)
{
	size_t count;

	count = pread(kpageflags_fd, flags, sizeof(*flags),
		      pfn * sizeof(*flags));

	if (count != sizeof(*flags))
		return -1;

	return 0;
}

void is_addr_thp(char *prefix, char *addr, int kpageflags_fd)
{
	const uint64_t folio_head_flags = KPF_THP | KPF_COMPOUND_HEAD;
	const uint64_t folio_tail_flags = KPF_THP | KPF_COMPOUND_TAIL;
	unsigned long pfn;
	uint64_t pfn_flags;

	pfn = pagemap_get_pfn(addr);
	pageflags_get(pfn, kpageflags_fd, &pfn_flags);

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

int pagemapcount_get(unsigned long pfn, uint64_t *mapcount)
{
	size_t count;
	int kpageflags_fd;

	kpageflags_fd = open(KPAGECOUNT_FILE_PATH, O_RDONLY);
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
