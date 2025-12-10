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

bool __check_huge(void *addr, char *pattern, int nr_hpages,
		  uint64_t hpage_size)
{
	char buffer[MAX_LINE_LENGTH];
	uint64_t thp = -1;
	char *entry;

	entry = __get_smap_entry(addr, pattern, buffer, sizeof(buffer));
	if (!entry)
		goto err_out;

	if (sscanf(entry, "%9" SCNu64 " kB", &thp) != 1)
		exit(-1);

err_out:
	return thp == (nr_hpages * (hpage_size >> 10));
}

bool check_huge_anon(void *addr, int nr_hpages, uint64_t hpage_size)
{
	return __check_huge(addr, "AnonHugePages: ", nr_hpages, hpage_size);
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

