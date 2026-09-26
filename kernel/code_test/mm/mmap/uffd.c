#define _GNU_SOURCE
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

static size_t page_size;

#define GREEN   "\033[32m"
#define RESET   "\033[0m"

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

int simple_uffd()
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

int main()
{
	if (geteuid() != 0) {
		printf("Run it as root!\n");
		exit(1);
	}

	page_size = sysconf(_SC_PAGESIZE);

	simple_uffd();

	return 0;
}
