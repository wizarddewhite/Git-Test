#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/userfaultfd.h>
#include <errno.h>
#include "vm_util.h"

#define SYSFS_THP "/sys/kernel/mm/transparent_hugepage"
#define CG "/sys/fs/cgroup/ttu_parent/ttu/"

static size_t page_size;
static size_t pmd_size;
static volatile int stop_handler;

#define GREEN   "\033[32m"
#define RESET   "\033[0m"

static unsigned long read_sysfs_ul(const char *path)
{
	int fd = open(path, O_RDONLY);
	char buf[64] = "0";
	ssize_t n;

	if (fd < 0)
		return 0;
	n = read(fd, buf, sizeof(buf) - 1);
	close(fd);
	if (n > 0)
		buf[n] = '\0';
	return strtoul(buf, NULL, 0);
}

/* get value in [] from "always [madvise] never" */
static int read_sysfs_cur(const char *path, char *out, size_t len)
{
	int fd = open(path, O_RDONLY);
	char raw[512];
	char *l, *r;
	ssize_t n;

	if (fd < 0)
		return -1;
	n = read(fd, raw, sizeof(raw) - 1);
	close(fd);
	if (n <= 0)
		return -1;
	raw[n] = '\0';
	l = strchr(raw, '[');
	r = strchr(raw, ']');
	if (!l || !r || r < l)
		return -1;
	*r = '\0';
	snprintf(out, len, "%s", l + 1);
	return 0;
}

// userfault handler
static void* simple_handler(void* arg)
{
	int uffd = *(int*)arg;
	struct uffd_msg msg;
	struct uffdio_copy copy;
	ssize_t nread;

	printf(GREEN "userfault thread start...\n" RESET);

	while (1) {
		// 1. waiting fault
		struct pollfd pollfd = { .fd = uffd, .events = POLLIN };
		
		int poll_result = poll(&pollfd, 1, -1);
		if (poll_result < 0) {
			perror("poll");
			break;
		}
		
		// 2. get page fault info
		nread = read(uffd, &msg, sizeof(msg));
		if (nread < 0) {
			if (errno == EAGAIN)
				continue;
			perror("read uffd_msg");
			break;
		}
		
		if (msg.event != UFFD_EVENT_PAGEFAULT) {
			fprintf(stderr, "event exception: %d\n", msg.event);
			continue;
		}
		
		// 3. parse info
		unsigned long fault_addr = msg.arg.pagefault.address;
		unsigned long fault_page = fault_addr & ~(page_size - 1);
		int is_write = (msg.arg.pagefault.flags & UFFD_PAGEFAULT_FLAG_WRITE) ? 1 : 0;
		// int page_index = (fault_addr - (unsigned long)region_start) / page_size;
		
		printf(GREEN "\thandle page fault: fault_addr=0x%lx, fault_page=x%lx, reason=%s\n" RESET,
		       fault_addr, fault_page, is_write ? "WRITE" : "READ");
		
		// 4. prepare page content
		// here we emulate some data
		void* page_data = malloc(page_size);
		if (!page_data) {
			perror("malloc page_data");
			break;
		}
		
		memset(page_data, 0, page_size);
		char* info = (char*)page_data;
		snprintf(info, 100, "page addr: 0x%lx, fault addr: 0x%lx", 
			fault_page, fault_addr);
		
		// 5. UFFDIO_COPY
		copy.dst = fault_page;
		copy.src = (unsigned long)page_data;
		copy.len = page_size;
		copy.mode = 0;
		copy.copy = 0;
		
		if (ioctl(uffd, UFFDIO_COPY, &copy) < 0) {
			perror("UFFDIO_COPY");
			free(page_data);
			break;
		}
		
		printf(GREEN "\tuserfault resolved: page 0x%lx\n" RESET, fault_page);
		free(page_data);
	}

	return NULL;
}

int anon_uffd()
{
	const size_t size = 4 * page_size;
	int uffd;
	pthread_t handler_thread;
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	void* region;
	char* ptr;
	int i, ret = 0;
	
	printf("=== Userfaultfd simple example ===\n");

	// 1. create userfaultfd
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd < 0) {
		perror("userfaultfd");
		return -1;
	}
	printf("userfaultfd: fd=%d\n", uffd);
	
	// 2. init API
	uffdio_api.api = UFFD_API;
	uffdio_api.features = 0;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) < 0) {
		perror("UFFDIO_API");
		return -1;
	}
	printf("API version: %llu\n", uffdio_api.api);
	
	// 3. alloc memory region
	region = mmap(NULL, size, PROT_READ | PROT_WRITE,
	              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
		perror("mmap");
		ret = -1;
		goto close;
	}
	printf("alloc memory region: addr=0x%lx, size=0x%lx\n", 
	       (unsigned long)region, size);
	
	// 4. register to userfaultfd
	uffdio_register.range.start = (unsigned long)region;
	uffdio_register.range.len = size;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) < 0) {
		perror("UFFDIO_REGISTER");
		ret = -1;
		goto unmap;
	}
	printf("successfully registered userfaultfd\n");
	
	// 5. start userfault handle thread
	if (pthread_create(&handler_thread, NULL, simple_handler, &uffd) != 0) {
		perror("pthread_create");
		ret = -1;
		goto unregister;
	}
	
	// 6. main thread: access and trigger fault
	printf("\n=== Start to access region ===\n");
	sleep(1);
	
	ptr = (char*)region;
	
	// read on first page
	printf("1. read 1st byte of 1st page: \n");
	char value1 = ptr[0];
	printf("val='%c' (ASCII=%d)\n", value1, value1);
	printf("val=%s\n", ptr);
	sleep(1);
	
	// write on 2nd page
	printf("2. write on 2nd page: \n");
	ptr[page_size + 4] = 'X';
	printf("write 'X' on offset %ld \n", page_size + 4);
	printf("val='%c' (ASCII=%d)\n", ptr[page_size + 4], ptr[page_size + 4]);
	printf("val=%s\n", &ptr[page_size]);
	printf("        ^--- changed to X\n");
	sleep(1);
	
	// write on 3rd/4th page (across page)
	printf("3. access page 3-4: \n");
	memset(ptr + 2 * page_size, 'A', 2 * page_size);
	printf("write %ld bytes 'A'\n", 2 * page_size);
	sleep(1);
	
	// verify written data
	printf("4. verify: page 3-4 are all 'A'\n");
	for (i = 2 * page_size; i < 4 * page_size; i++) {
		if (ptr[i] != 'A') {
			printf("Is not A\n");
			break;
		}
	}
	
	// 7. cleanup
	printf("\n=== cleanup ===\n");
	sleep(2);
	
	// wait for fault handler thread
	pthread_cancel(handler_thread);
	pthread_join(handler_thread, NULL);
	
unregister:
	ioctl(uffd, UFFDIO_UNREGISTER, &uffdio_register.range);
unmap:
	munmap(region, size);
close:
	close(uffd);
	return ret;
}


static void *memfd_handler(void *arg)
{
	char *src = NULL;
	int uffd = *(int*)arg;

		src = aligned_alloc(page_size, page_size);
		if (!src) {
			perror("aligned_alloc");
			return NULL;
		}

	for (;;) {
		struct pollfd pfd = { .fd = uffd, .events = POLLIN };
		struct uffd_msg msg;
		struct uffdio_copy copy;
		// struct uffdio_zeropage zp;
		unsigned long addr;
		ssize_t n;
		int ret;

		/* poll with timeout */
		ret = poll(&pfd, 1, 100);
		if (ret == 0) {
			if (stop_handler)
				break;
			continue;
		}
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}
		if (pfd.revents & POLLERR) {
			fprintf(stderr, "uffd POLLERR\n");
			break;
		}

		n = read(uffd, &msg, sizeof(msg));
		if (n <= 0) {
			if (n < 0 && errno == EAGAIN)
				continue;
			break;
		}
		if (msg.event != UFFD_EVENT_PAGEFAULT)
			continue;

		/* get fault addr */
		addr = msg.arg.pagefault.address & ~(unsigned long)(page_size - 1);


			snprintf(src, page_size, "  memfd uffd fault at %lu", addr);
			copy.dst  = addr;
			copy.src  = (unsigned long)src;
			copy.len  = page_size;
			copy.mode = 0;
			if (ioctl(uffd, UFFDIO_COPY, &copy)) {
				fprintf(stderr, "UFFDIO_COPY failed: %s\n", strerror(errno));
				break;
			}
	}

	free(src);

	return NULL;
}

/*
 * Use memfd as uffd register backend.
 *
 * @pre_fault: fault memfd before uffd registe
 */
int memfd_uffd(bool pre_fault)
{
	struct uffdio_api api = { 0 };
	struct uffdio_register reg = { 0 };
	size_t region_len = 4UL << 20;
	unsigned long start_pfn = -1UL;
	char *region;
	unsigned long nr_pages;
	int uffd, memfd;
	pthread_t thr;

	nr_pages = region_len / page_size;

	printf("page size %zu, PMD %zu, region_len %zu (%lu pages)\n",
	       page_size, pmd_size, region_len, nr_pages);

	/* 1. memfd */
	memfd = memfd_create("uffd_test", MFD_CLOEXEC);
	if (memfd < 0) {
		perror("memfd_create");
		return -1;
	}
	if (ftruncate(memfd, region_len)) {
		perror("ftruncate");
		return -1;
	}

	/* 1.1. map with PMD aligned*/
	{
		void *res = mmap(NULL, region_len + pmd_size, PROT_NONE,
				 MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
		if (res == MAP_FAILED) {
			perror("mmap reserve");
			return -1;
		}
		uintptr_t aligned = ((uintptr_t)res + pmd_size - 1) & ~(uintptr_t)(pmd_size - 1);
		region = mmap((void *)aligned, region_len, PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_FIXED, memfd, 0);
		if (region == MAP_FAILED) {
			perror("mmap memfd");
			return -1;
		}
	}
	printf("map memfd at %p (PMD aligned)\n", region);

	/* 1.2 pre-fault it */
	if (pre_fault) {
		int pages;

		if (madvise(region, region_len, MADV_HUGEPAGE)) {
			perror("madvise(MADV_HUGEPAGE)");
			return -1;
		}

		for (char *p = region; p < region + region_len; p += page_size)
			*p = 'p';

		start_pfn = pagemap_get_pfn(region);
		pages = vaddr_page_number(region, page_size);
		printf("page(%lx) mapped at @region is %s folio, %d\n",
				start_pfn, pages > 1 ? "large":"base", pages);
	}

	/* 2. userfaultfd */
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd < 0) {
		perror("userfaultfd");
		return -1;
	}

	api.api = UFFD_API;
	api.features = UFFD_FEATURE_MISSING_SHMEM;
	if (ioctl(uffd, UFFDIO_API, &api)) {
		perror("UFFDIO_API");
		return -1;
	}
	if (!(api.features & UFFD_FEATURE_MISSING_SHMEM)) {
		fprintf(stderr, "MISSING_SHMEM not supported\n");
		return -1;
	}

	reg.range.start = (unsigned long)region;
	reg.range.len   = region_len;
	reg.mode        = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &reg)) {
		perror("UFFDIO_REGISTER");
		fprintf(stderr, "  (EINVAL: VMA is not MISSING compatible?)\n");
		return -1;
	}
	printf("\n=== uffd registered ===\n");
	printf("available ioctl: COPY=%d ZEROPAGE=%d CONTINUE=%d WAKE=%d\n",
	       !!(reg.ioctls & (1ULL << _UFFDIO_COPY)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_ZEROPAGE)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_CONTINUE)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_WAKE)));
	printf("Note: CONTINUE=%d -- CONTINUE is meaningful under MINOR mode\n",
	       !!(reg.ioctls & (1ULL << _UFFDIO_CONTINUE)));

	/* 3. start uffd handler */
	if (pthread_create(&thr, NULL, memfd_handler, &uffd)) {
		perror("pthread_create");
		return -1;
	}

	/* 4. trigger fault */
	region[1] = 'A';
	region[page_size + 1] = 'B';
	stop_handler = 1;
	pthread_join(thr, NULL);
	if (pre_fault) {
		if (start_pfn != pagemap_get_pfn(region))
			printf("region mapped to different pfn %lx\n", pagemap_get_pfn(region));
		else
			printf("region mapped to same pfn %lx\n", start_pfn);
	}
	printf("nr_pages at @region is %d\n", vaddr_page_number(region, page_size));
	printf("nr_pages at @region is %d\n", vaddr_page_number(region + page_size, page_size));

	printf("content: %s\n", region);
	printf("content: %s\n", region + page_size);

	return 0;
}

static void write_memory_reclaim(const char *p, const char *v)
{
	int fd = open(p, O_WRONLY);
	int ret;

	if (fd < 0) {
		fprintf(stderr, "  write %s fail: %s\n", p, strerror(errno));
		return;
	}
	ret = write(fd, v, strlen(v));
	close(fd);

	if (ret < 0) {
		if (errno == EAGAIN)
			printf("partially reclaimed\n");
		else if (errno == EINVAL)
			printf("invalid format\n");
		else
			perror("write failed");
	}
}

/*
 * Prepare cgroup:
 *
 *  mkdir -p /sys/fs/cgroup/ttu_parent
 *  echo "+memory" > /sys/fs/cgroup/ttu_parent/cgroup.subtree_control 2>/dev/null
 *  mkdir -p /sys/fs/cgroup/ttu_parent/ttu
 *  CG=/sys/fs/cgroup/ttu_parent/ttu
 *
 * Run test in cgroup:
 *
 *  sudo sh -c 'echo $$ > /sys/fs/cgroup/ttu_parent/ttu/cgroup.procs && exec ./uffd'
 *
 */
int uffd_faulted_memfd()
{
	struct uffdio_api api = { 0 };
	struct uffdio_register reg = { 0 };
	char memory_reclaim[64] = "100M";
	size_t region_len = 4UL << 20;
	unsigned long start_pfn = -1UL;
	char *memfd_region, *uffd_region;
	unsigned long nr_pages;
	int uffd, memfd;
	pthread_t thr;

	nr_pages = region_len / page_size;

	printf("page size %zu, PMD %zu, region_len %zu (%lu pages)\n",
	       page_size, pmd_size, region_len, nr_pages);

	/* 1. memfd */
	memfd = memfd_create("uffd_test", MFD_CLOEXEC);
	if (memfd < 0) {
		perror("memfd_create");
		return -1;
	}
	if (ftruncate(memfd, region_len)) {
		perror("ftruncate");
		return -1;
	}

	/* 1.1. map with PMD aligned*/
	{
		void *res = mmap(NULL, region_len + pmd_size, PROT_NONE,
				 MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
		if (res == MAP_FAILED) {
			perror("mmap reserve");
			return -1;
		}
		uintptr_t aligned = ((uintptr_t)res + pmd_size - 1) & ~(uintptr_t)(pmd_size - 1);
		memfd_region = mmap((void *)aligned, region_len, PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_FIXED, memfd, 0);
		if (memfd_region == MAP_FAILED) {
			perror("mmap memfd");
			return -1;
		}
	}
	printf("map memfd at %p (PMD aligned)\n", memfd_region);

	/* 1.2 fault it */
	if (madvise(memfd_region, region_len, MADV_HUGEPAGE)) {
		perror("madvise(MADV_HUGEPAGE)");
		return -1;
	}

	for (char i = 'a', *p = memfd_region; p < memfd_region + region_len; p += page_size, i++)
		*p = i;

	start_pfn = pagemap_get_pfn(memfd_region);
	nr_pages = vaddr_page_number(memfd_region, page_size);
	printf("page(%lx) mapped at @memfd_region is %s folio, %ld\n",
			start_pfn, nr_pages > 1 ? "large":"base", nr_pages);
	printf("ShmemPmdMapped %lu kB\n", get_shmem_pmd_mapped(memfd_region));
	munmap(memfd_region, region_len);

	/* 2. userfaultfd */

	/* 2.1 create region for uffd */
	{
		void *res = mmap(NULL, region_len + pmd_size, PROT_NONE,
				 MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
		if (res == MAP_FAILED) {
			perror("mmap reserve");
			return -1;
		}
		uintptr_t aligned = ((uintptr_t)res + pmd_size - 1) & ~(uintptr_t)(pmd_size - 1);
		uffd_region = mmap((void *)aligned, region_len, PROT_READ | PROT_WRITE,
			      MAP_SHARED | MAP_FIXED, memfd, 0);
		if (uffd_region == MAP_FAILED) {
			perror("mmap memfd");
			return -1;
		}
	}
	printf("map uffd at %p (PMD aligned)\n", uffd_region);


	/* 2.2 register uffd */
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd < 0) {
		perror("userfaultfd");
		return -1;
	}

	api.api = UFFD_API;
	api.features = UFFD_FEATURE_MISSING_SHMEM;
	if (ioctl(uffd, UFFDIO_API, &api)) {
		perror("UFFDIO_API");
		return -1;
	}
	if (!(api.features & UFFD_FEATURE_MISSING_SHMEM)) {
		fprintf(stderr, "MISSING_SHMEM not supported\n");
		return -1;
	}

	reg.range.start = (unsigned long)uffd_region;
	reg.range.len   = region_len;
	reg.mode        = UFFDIO_REGISTER_MODE_MISSING | UFFDIO_REGISTER_MODE_WP;
	if (ioctl(uffd, UFFDIO_REGISTER, &reg)) {
		perror("UFFDIO_REGISTER");
		fprintf(stderr, "  (EINVAL: VMA is not MISSING compatible?)\n");
		return -1;
	}
	printf("\n=== uffd registered ===\n");
	printf("available ioctl: COPY=%d ZEROPAGE=%d CONTINUE=%d WAKE=%d\n",
	       !!(reg.ioctls & (1ULL << _UFFDIO_COPY)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_ZEROPAGE)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_CONTINUE)),
	       !!(reg.ioctls & (1ULL << _UFFDIO_WAKE)));
	printf("Note: CONTINUE=%d -- CONTINUE is meaningful under MINOR mode\n",
	       !!(reg.ioctls & (1ULL << _UFFDIO_CONTINUE)));

	/* 3. start uffd handler */
	if (pthread_create(&thr, NULL, memfd_handler, &uffd)) {
		perror("pthread_create");
		return -1;
	}

	/* 4. trigger fault */
	printf("content: %s\n", uffd_region);
	printf("content: %s\n", uffd_region + page_size * 1);
	printf("content: %s\n", uffd_region + page_size * 2);
	printf("content: %s\n", uffd_region + page_size * 3);
	printf("content: %s\n", uffd_region + page_size * 4);
	// uffd_region[1] = 'A';
	stop_handler = 1;
	pthread_join(thr, NULL);
	if (start_pfn != pagemap_get_pfn(uffd_region))
		printf("region mapped to different pfn %lx\n", pagemap_get_pfn(uffd_region));
	else
		printf("region mapped to same pfn %lx\n", start_pfn);
	printf("nr_pages at @uffd_region is %d\n", vaddr_page_number(uffd_region, page_size));

	/* inactivate + clear young */
	if (madvise(uffd_region, region_len, MADV_COLD)) {
		perror("madvise(MADV_COLD)");
		return -1;
	}

	/* trigger reclaim */
	printf("--- current before %lu\n", read_sysfs_ul(CG "memory.current"));
	printf("        echo %s > " CG "memory.reclaim\n", memory_reclaim);
	write_memory_reclaim(CG "memory.reclaim", memory_reclaim);
	printf("--- current after %lu\n", read_sysfs_ul(CG "memory.current"));

	return 0;
}

int main()
{
	char buf[256];

	if (geteuid() != 0) {
		printf("Run it as root!\n");
		exit(1);
	}

	page_size = sysconf(_SC_PAGESIZE);
	pmd_size = read_sysfs_ul(SYSFS_THP "/hpage_pmd_size");
	if (!pmd_size) {
		printf("Reading PMD pagesize failed");
		return -1;
	}

	if (read_sysfs_cur(SYSFS_THP "/shmem_enabled", buf, sizeof(buf)) == 0) {
		printf("shmem_enabled: %s\n", buf);
		if (!strcmp(buf, "deny") || !strcmp(buf, "never")) {
			printf("\n\tshmem_enabled should not be deny/never\n");
			return -1;
		}
	} else {
		printf("shmem_enabled not exist\n");
		return -1;
	}

	// anon_uffd();
	// memfd_uffd(false);
	// memfd_uffd(true);
	uffd_faulted_memfd();

	return 0;
}
