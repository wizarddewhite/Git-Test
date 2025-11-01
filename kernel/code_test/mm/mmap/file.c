#include <fcntl.h>      // open()
#include <sys/mman.h>   // mmap(), munmap()
#include <sys/stat.h>   // fstat()
#include <unistd.h>     // close(), ftruncate(), unlink()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PATH "/tmp/mmap_example.txt"
#define MAP_SIZE 4096

int main()
{
	int fd;
	char *mapped;

	// 创建或打开文件
	fd = open(FILE_PATH, O_RDWR | O_CREAT | O_EXCL, 0666);  // O_EXCL 防止重复打开
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	// 设置文件大小
	if (ftruncate(fd, MAP_SIZE) == -1) {
		perror("ftruncate");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// 映射文件到内存
	mapped = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mapped == MAP_FAILED) {
		perror("mmap");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("Memory mapped at address %p\n", mapped);

	// 写入数据到映射内存
	const char *msg = "Hello from mmap!";
	strncpy(mapped, msg, strlen(msg));
	printf("Written: %s\n", msg);

	// 关闭文件描述符（此时文件依然存在）
	close(fd);

	// 从内存中读取数据
	printf("Read from memory-mapped file: %s\n", mapped);

	// 解除内存映射
	if (munmap(mapped, MAP_SIZE) == -1) {
		perror("munmap");
		exit(EXIT_FAILURE);
	}

	// 删除文件（即使已关闭，仍可通过路径删除）
	if (unlink(FILE_PATH) == -1) {
		perror("unlink");
		exit(EXIT_FAILURE);
	}

	printf("File '%s' has been deleted.\n", FILE_PATH);

	return 0;
}
