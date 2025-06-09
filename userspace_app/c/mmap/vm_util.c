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
#include "vm_util.h"

uint64_t pagemap_get_entry(int fd, char *start)
{
	const unsigned long pfn = (unsigned long)start / getpagesize();
	uint64_t entry;
	int ret;

	ret = pread(fd, &entry, sizeof(entry), pfn * sizeof(entry));
	if (ret != sizeof(entry))
		printf("reading pagemap failed\n");
	return entry;
}

unsigned long pagemap_get_pfn(int fd, char *start)
{
	uint64_t entry = pagemap_get_entry(fd, start);

	/* If present (63th bit), PFN is at bit 0 -- 54. */
	if (entry & PM_PRESENT)
		return entry & 0x007fffffffffffffull;
	return -1ul;
}
