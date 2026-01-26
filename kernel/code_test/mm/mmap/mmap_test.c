#define _GNU_SOURCE
#include <fcntl.h>      // open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <malloc.h>
#include "vm_util.h"

void read_mmap()
{
	char buffer[100];
	FILE * f = fopen("/proc/self/maps", "rb");

	if (f) {
		while (fgets(buffer, sizeof(buffer), f)) {
			printf("%s", buffer);
		}
		fclose(f);
	} else {
		printf("file open fail\n");
	}
}

void map_unmap_move()
{
	size_t pagesize = 1 << 12;
	size_t mapsize = pagesize * 3;
	int unmap_result;

	printf("System page size: %lx bytes\n", pagesize);
	printf("### Before mmap:\n");
	read_mmap();

	getchar();

	/* Map a 3 page range with kernel chosen start address */
	char *region = mmap(
	  NULL,					/* kernel choose */
	  mapsize,				/* map size */
	  PROT_READ|PROT_WRITE|PROT_EXEC,	/* protection */
	  MAP_ANONYMOUS|MAP_PRIVATE,		/* flags */
	  -1,
	  0
	);
	if (region == MAP_FAILED) {
		perror("Could not mmap");
		return;
	}

	printf("### After mmap: [%p - %p]\n", region, region + mapsize);
	read_mmap();

	getchar();

	// write some content to 2nd page
	strcpy(region + pagesize, "Hello, world!");
	*(region + 2 * pagesize - 1) = 'W';

	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	// unmap the 1st page
	unmap_result = munmap(region, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return;
	}
	// unmap the 3rd page
	unmap_result = munmap(region + 2 * pagesize, pagesize);
	if (unmap_result) {
		perror("Could not munmap");
		return;
	}

	printf("### After munmap: [%p - %p] [%p - %p]\n",
		region, region + pagesize, region + 2 * pagesize, region + 3 * pagesize);
	read_mmap();
	// show the content still there
	printf("Expect left region: [%p - %p]\n", region + pagesize, region + pagesize * 2);
	printf("Contents of region: %s %c\n",
		  region + pagesize, *(region + 2 * pagesize - 1));

	getchar();

	// move one page shift, can't overlap
	char *new_addr = region + mapsize + pagesize;
	char *move_region = mremap(
			region + pagesize,		/* old_addr */
			pagesize,			/* old_size */
			pagesize,			/* new_size */
			MREMAP_FIXED | MREMAP_MAYMOVE,	/* flags */
			new_addr			/* new_addr */
		);
	if (move_region == MAP_FAILED) {
		perror("Could not remap");
		return;
	}

	if (move_region != new_addr) {
		printf("mremap failed to move to specified target\n");
		return;
	}

	printf("Move to region: [%p - %p] -> [%p - %p]\n", 
		region + pagesize, region + 2 * pagesize, move_region, move_region + pagesize);
	printf("Contents of moved region: %s %c\n",
		  move_region, *(move_region + pagesize - 1));
	read_mmap();

	getchar();
}

void map_file()
{
#define FILE_PATH "/tmp/mmap_example.txt"
#define MAP_SIZE 4096

	int fd;
	char *mapped;

	// 创建或打开文件
	fd = open(FILE_PATH, O_RDWR | O_CREAT | O_EXCL, 0666);  // O_EXCL 防止重复打开
	if (fd == -1) {
		perror("open");
		return;
	}

	// 设置文件大小
	if (ftruncate(fd, MAP_SIZE) == -1) {
		perror("ftruncate");
		close(fd);
		return;
	}

	// 映射文件到内存
	// 这里一定要用MAP_SHARED，否则内容不会真实写到文件里
	// 而且不能加MAP_ANONYMOUS，否则fd参数就失效，实际是映射内存
	mapped = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapped == MAP_FAILED) {
		perror("mmap");
		close(fd);
		return;
	}
	// 关闭文件描述符（此时文件依然存在）
	close(fd);


	printf("File mapped at address %p\n", mapped);

	// 写入数据到映射内存
	const char *msg = "Hello from mmap!";
	strncpy(mapped, msg, strlen(msg));
	printf("Written: %s\n", msg);

	// 从内存中读取数据
	printf("Read from memory-mapped file: %s\n", mapped);

	// 解除内存映射
	if (munmap(mapped, MAP_SIZE) == -1) {
		perror("munmap");
		return;
	}

	// 删除文件（即使已关闭，仍可通过路径删除）
	if (unlink(FILE_PATH) == -1) {
		perror("unlink");
		return;
	}

	printf("File '%s' has been deleted.\n", FILE_PATH);

}

void map_shm()
{
	const char *name = "shm_example";
	const size_t SIZE = 4096;

	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // 打开共享内存对象
	if (shm_fd == -1) {
		perror("shm_open");
		return ;
	}

	ftruncate(shm_fd, SIZE);

	// 和文件打开参数一样，这里要用MAP_SHARED，否则内容不会真实写到文件里
	// 也就是说如果用shm做进程间数据传输，用MAP_PRIVATE会没有效果
	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // 映射到进程地址空间
	if (ptr == MAP_FAILED) {
		perror("mmap");
		return ;
	}

	sprintf((char*) ptr, "Hello from shared memory!");

	printf("Data read from shared memory: %s\n",(char*)ptr);

	munmap(ptr, SIZE); // 解除映射

	// 移除共享内存对象
	// 该文件在/dev/shm/shm_example
	shm_unlink(name);
}

void map_file_private_shared()
{
	struct pagemap_info shared_info, private_info;
	if (geteuid() != 0) {
		printf("Please run as root\n");
		return;
	}

	int fd = open("test.dat", O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		perror("open");
		return;
	}

	// 准备测试文件内容
	const char *initial_content = "Hello, World!This is init data";
	ftruncate(fd, strlen(initial_content) + 1);
	write(fd, initial_content, strlen(initial_content) + 1);

	// 映射文件
	char *shared_map = mmap(NULL, 100, PROT_READ | PROT_WRITE,
			       MAP_SHARED, fd, 0);
	char *private_map = mmap(NULL, 100, PROT_READ | PROT_WRITE,
				MAP_PRIVATE, fd, 0);

	if (shared_map == MAP_FAILED || private_map == MAP_FAILED) {
	    perror("mmap");
	    return;
	}

	shared_info.addr = shared_map;
	private_info.addr = private_map;

	printf("Initial content:\n");
	printf("shared map: %s\n", shared_map);
	printf("privat map: %s\n", private_map);

	// 测试写操作的影响
	printf("\n=== test shared map write ===\n");
	memcpy(shared_map, "SHARED modify", strlen("SHARED modify"));
	printf("after - shared map: %s\n", shared_map);
	printf("after - privat map: %s\n", private_map);  // 私有映射应该不变
	// 这里可以看到，shared_map和private_map背后的页面是一致的，
	// 所以对共享页面的改动，能够反映到私有页面上
	pagemap_get_info(&shared_info);
	pagemap_get_info(&private_info);
	printf("pfn behined shared %lx is %s\n", shared_info.pfn, shared_info.is_file ? "file":"anon");
	printf("pfn behined privat %lx is %s\n", private_info.pfn, private_info.is_file ? "file":"anon");

	// 重置内容
	lseek(fd, 0, SEEK_SET);
	write(fd, initial_content, strlen(initial_content) + 1);

	printf("\nReset content:\n");
	printf("shared map: %s\n", shared_map);
	printf("privat map: %s\n", private_map);


	printf("\n=== test private map write ===\n");
	memcpy(private_map, "PRIVATE modify", strlen("PRIVATE modify"));
	printf("after - shared map: %s\n", shared_map);  // 应该还是初始内容
	printf("after - privat map: %s\n", private_map);
	// 这里看到shared_map和private_map背后的页面已经不一样了
	pagemap_get_info(&shared_info);
	pagemap_get_info(&private_info);
	printf("pfn behined shared %lx is %s\n", shared_info.pfn, shared_info.is_file ? "file":"anon");
	printf("pfn behined privat %lx is %s\n", private_info.pfn, private_info.is_file ? "file":"anon");

	printf("\n=== test shared map write after cow ===\n");
	strcpy(shared_map, "Shared modify after cow");
	printf("after - shared map: %s\n", shared_map);
	printf("after - privat map: %s\n", private_map);

	// 私有映射页面第二次写入，最后呈现的内容在上一次写的结果上
	printf("\n=== test private map 2nd write ===\n");
	memcpy(private_map, "2nd write", strlen("2nd write"));
	printf("after - shared map: %s\n", shared_map);  // 应该还是初始内容
	printf("after - privat map: %s\n", private_map);
	// 这里看到shared_map和private_map背后的页面已经不一样了
	pagemap_get_info(&shared_info);
	pagemap_get_info(&private_info);
	printf("pfn behined shared %lx is %s\n", shared_info.pfn, shared_info.is_file ? "file":"anon");
	printf("pfn behined privat %lx is %s\n", private_info.pfn, private_info.is_file ? "file":"anon");

	munmap(shared_map, 100);
	munmap(private_map, 100);
	close(fd);
	unlink("test.dat");
}

void map_anon_private_shared()
{
	struct pagemap_info shared_info, private_info;
	char *shared_map, *private_map;

	if (geteuid() != 0) {
		printf("Please run as root\n");
		return;
	}

	shared_map = mmap(NULL, 1 << 12, PROT_READ | PROT_WRITE,
			       MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	private_map = mmap(NULL, 1 << 12, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (shared_map == MAP_FAILED || private_map == MAP_FAILED) {
	    perror("mmap");
	    return;
	}
	shared_info.addr = shared_map;
	private_info.addr = private_map;

	// fault in
	strcpy(shared_map, "shared write");
	strcpy(private_map, "private write");
	printf("Initial content:\n");
	printf("shared map: %s\n", shared_map);
	printf("privat map: %s\n", private_map);

	pagemap_get_info(&shared_info);
	pagemap_get_info(&private_info);
	printf("pfn behined shared %lx is %s\n", shared_info.pfn, shared_info.is_file ? "file":"anon");
	printf("pfn behined privat %lx is %s\n", private_info.pfn, private_info.is_file ? "file":"anon");
}

void map_anon_thp()
{
	const uint64_t folio_head_flags = KPF_THP | KPF_COMPOUND_HEAD;
	const uint64_t folio_tail_flags = KPF_THP | KPF_COMPOUND_TAIL;
	const char *kpageflags_proc = "/proc/kpageflags";
	char *one_page;
	uint64_t pmd_pagesize;
	uint64_t pagesize;
	unsigned long pfn;
	int kpageflags_fd;
	uint64_t pfn_flags;

	kpageflags_fd = open(kpageflags_proc, O_RDONLY);
	if (kpageflags_fd == -1)
		exit(-1);

	pagesize = getpagesize();
	pmd_pagesize = read_pmd_pagesize();

	one_page = memalign(pmd_pagesize, 2 * pmd_pagesize);
	madvise(one_page, 2 * pmd_pagesize, MADV_HUGEPAGE);

	if (!check_huge_anon(one_page, 0, pmd_pagesize)) {
		printf("We don't expect huge anon page, but have\n");
	} else {
		printf("===After madvise, there is no huge anon yet\n");
	}

	/* fault in page */
	one_page[0] = 0;
	one_page[pmd_pagesize] = 1;

	if (!check_huge_anon(one_page, 2, pmd_pagesize)) {
		printf("We expect huge anon page, but not\n");
	} else {
		printf("===After fault in, there is huge anon yet\n");
	}

	pfn = pagemap_get_pfn(one_page);
	pageflags_get(pfn, kpageflags_fd, &pfn_flags);

	if ((pfn_flags & folio_head_flags) == folio_head_flags)
		printf("vaddr(%lx) at pfn(%lx) is Head\n",
				(unsigned long)one_page, pfn);

	pfn = pagemap_get_pfn(one_page + pagesize);
	pageflags_get(pfn, kpageflags_fd, &pfn_flags);

	if ((pfn_flags & folio_tail_flags) == folio_tail_flags)
		printf("vaddr(%lx) at pfn(%lx) is Tail\n",
				(unsigned long)one_page + pagesize, pfn);

	close(kpageflags_fd);
}

void show_pfn_thp(char *prefix, char *addr, int kpageflags_fd)
{
	const uint64_t folio_head_flags = KPF_THP | KPF_COMPOUND_HEAD;
	const uint64_t folio_tail_flags = KPF_THP | KPF_COMPOUND_TAIL;
	unsigned long pfn;
	uint64_t pfn_flags;

	pfn = pagemap_get_pfn(addr);
	pageflags_get(pfn, kpageflags_fd, &pfn_flags);

	if (!(pfn_flags & KPF_THP))
		printf("%svaddr(%lx) at pfn(%lx) isn't THP\n",
			prefix, (unsigned long)addr, pfn);

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

void unmap_partial_anon_thp()
{
	const char *kpageflags_proc = "/proc/kpageflags";
	char *one_page;
	uint64_t pmd_pagesize;
	uint64_t pagesize;
	int kpageflags_fd;
	pid_t pid;
	int ret;

	printf("parent %d\n", getpid());
	kpageflags_fd = open(kpageflags_proc, O_RDONLY);
	if (kpageflags_fd == -1)
		exit(-1);

	pagesize = getpagesize();
	pmd_pagesize = read_pmd_pagesize();

	one_page = memalign(pmd_pagesize, 2 * pmd_pagesize);
	madvise(one_page, 2 * pmd_pagesize, MADV_HUGEPAGE);

	if (!check_huge_anon(one_page, 0, pmd_pagesize)) {
		printf("We don't expect huge anon page, but have\n");
	} else {
		printf("===After madvise, there is no huge anon yet\n");
	}

	/* fault in page */
	one_page[0] = 3;
	one_page[pmd_pagesize] = 1;

	show_pfn_thp("\tparent ", one_page, kpageflags_fd);
	show_pfn_thp("\tparent ", one_page + pmd_pagesize, kpageflags_fd);

	pid = fork();
	if (pid < 0) {
		perror("fork\n");
	} else if (pid == 0) {
		printf("..in child %d\n", getpid());

		// check the content is the same
		if (one_page[0] != 3) {
			printf("content in one_page[0] is changed\n");
			exit(0);
		}

		if (!check_huge_anon(one_page, 2, pmd_pagesize)) {
			printf("We expect huge anon page, but not\n");
		} else {
			printf("===After fork, there is still huge anon %lukb\n",
				(2 * (pmd_pagesize >> 10)));
		}

		// check the range is still thp
		show_pfn_thp("\tchild ", one_page, kpageflags_fd);
		show_pfn_thp("\tchild ", one_page + pmd_pagesize, kpageflags_fd);

		// unmap a part of the thp
		ret = munmap(one_page + pagesize, pagesize);
		if (ret) {
			perror("munmap\n");
			exit(0);
		}

		// check the folio is still thp
		printf("===After unmap part range\n");
		show_pfn_thp("\tchild ", one_page, kpageflags_fd);
		show_pfn_thp("\tchild ", one_page + pmd_pagesize, kpageflags_fd);

		// first range just contain small folio
		printf("\tchild first range has huge anon %lukb, anon %lukb\n",
			get_huge_anon(one_page), get_anon(one_page));
		// second range contain small folio and one thp
		printf("\tchild second range has huge anon %lukb, anon %lukb\n",
			get_huge_anon(one_page + 2 * pagesize),
			get_anon(one_page + 2 * pagesize));
	} else {
		wait(NULL);
		printf("===child quit\n");
		printf("\tparent range has huge anon %lukb, anon %lukb\n",
			get_huge_anon(one_page), get_anon(one_page));
	}

	close(kpageflags_fd);
}
 
int main(void) {
	// map_unmap_move();
	// map_file();
	// map_shm();
	// map_file_private_shared();
	// map_anon_private_shared();
	// map_anon_thp();
	unmap_partial_anon_thp();
	return 0;
}
