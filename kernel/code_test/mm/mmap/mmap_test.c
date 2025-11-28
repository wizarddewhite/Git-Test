#define _GNU_SOURCE
#include <fcntl.h>      // open()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

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
 
int main(void) {
	// map_unmap_move();
	map_file();
	return 0;
}
