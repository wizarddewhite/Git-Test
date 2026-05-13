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

#define PAGE_SIZE 4096
#define REGION_SIZE (4 * PAGE_SIZE)

#define GREEN   "\033[32m"
#define RESET   "\033[0m"

// userfault handler
static void* fault_handler_thread(void* arg)
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
		    fprintf(stderr, "exception: %d\n", msg.event);
		    continue;
		}
		
		// 3. parse info
		unsigned long fault_addr = msg.arg.pagefault.address;
		unsigned long fault_page = fault_addr & ~(PAGE_SIZE - 1);
		int is_write = (msg.arg.pagefault.flags & UFFD_PAGEFAULT_FLAG_WRITE) ? 1 : 0;
		// int page_index = (fault_addr - (unsigned long)region_start) / PAGE_SIZE;
		
		printf(GREEN "handle page fault: fault_addr=0x%lx, fault_page=x%lx, reason=%s\n" RESET,
		       fault_addr, fault_page, is_write ? "WRITE" : "READ");
		
		// 4. prepare page content
		// here we emulate some data
		void* page_data = malloc(PAGE_SIZE);
		if (!page_data) {
		    perror("malloc page_data");
		    break;
		}
		
		memset(page_data, 0, PAGE_SIZE);
		char* info = (char*)page_data;
		snprintf(info, 100, "page addr: 0x%lx, fault addr: 0x%lx", 
			fault_page, fault_addr);
		
		// 5. UFFDIO_COPY
		copy.dst = fault_page;
		copy.src = (unsigned long)page_data;
		copy.len = PAGE_SIZE;
		copy.mode = 0;
		copy.copy = 0;
		
		if (ioctl(uffd, UFFDIO_COPY, &copy) < 0) {
		    perror("UFFDIO_COPY");
		    free(page_data);
		    break;
		}
		
		printf(GREEN "userfault resolved: page 0x%lx\n" RESET, fault_page);
		free(page_data);
	}

	return NULL;
}

int main()
{
	int uffd;
	pthread_t handler_thread;
	void* region;
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	char* ptr;
	
	printf("=== Userfaultfd example ===\n");
	
	// 1. create userfaultfd
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd < 0) {
	    perror("userfaultfd");
	    exit(1);
	}
	printf("userfaultfd: fd=%d\n", uffd);
	
	// 2. init API
	uffdio_api.api = UFFD_API;
	uffdio_api.features = 0;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) < 0) {
	    perror("UFFDIO_API");
	    close(uffd);
	    exit(1);
	}
	printf("API version: %llu\n", uffdio_api.api);
	
	// 3. alloc memory region
	region = mmap(NULL, REGION_SIZE, PROT_READ | PROT_WRITE,
	              MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
	    perror("mmap");
	    close(uffd);
	    exit(1);
	}
	printf("alloc memory region: addr=0x%lx, size=0x%x\n", 
	       (unsigned long)region, REGION_SIZE);
	
	// 4. register to userfaultfd
	uffdio_register.range.start = (unsigned long)region;
	uffdio_register.range.len = REGION_SIZE;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) < 0) {
	    perror("UFFDIO_REGISTER");
	    munmap(region, REGION_SIZE);
	    close(uffd);
	    exit(1);
	}
	printf("successfully registered userfaultfd\n");
	
	// 5. start userfault handle thread
	if (pthread_create(&handler_thread, NULL, fault_handler_thread, &uffd) != 0) {
	    perror("pthread_create");
	    ioctl(uffd, UFFDIO_UNREGISTER, &uffdio_register.range);
	    munmap(region, REGION_SIZE);
	    close(uffd);
	    exit(1);
	}
	
	// 6. main thread: access and trigger fault
	printf("\n=== Start to access region ===\n");
	sleep(1);
	
	ptr = (char*)region;
	
	// read on first page
	printf("1. read 1st byte of 1st page: ");
	char value1 = ptr[0];
	printf("val='%c' (ASCII=%d)\n", value1, value1);
	sleep(1);
	
	// write on 2nd page
	printf("2. write on 2nd page: ");
	ptr[PAGE_SIZE + 100] = 'X';
	printf("write 'X' on offset %d \n", PAGE_SIZE + 100);
	sleep(1);
	
	// 触发第3个页面（跨页面访问）
	printf("3. access page 3-4: ");
	memset(ptr + 2 * PAGE_SIZE, 'A', 2 * PAGE_SIZE);
	printf("write %d bytes 'A'\n", 2 * PAGE_SIZE);
	sleep(1);
	
	// verify written data
	printf("4. verify: offset %d ='%c'\n", PAGE_SIZE + 100, ptr[PAGE_SIZE + 100]);
	
	// 7. cleanup
	printf("\n=== cleanup ===\n");
	sleep(2);
	
	// 取消注册
	if (ioctl(uffd, UFFDIO_UNREGISTER, &uffdio_register.range) < 0) {
	    perror("UFFDIO_UNREGISTER");
	} else {
	    printf("unregister uffd area\n");
	}
	
	// wait for fault handler thread
	pthread_cancel(handler_thread);
	pthread_join(handler_thread, NULL);
	
	// cleanup
	munmap(region, REGION_SIZE);
	close(uffd);
	
	return 0;
}
