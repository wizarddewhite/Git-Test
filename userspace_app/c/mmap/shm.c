#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
	const char *name = "shm_example";
	const size_t SIZE = 4096;

	int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666); // 打开共享内存对象
	if (shm_fd == -1) {
		perror("shm_open");
		return 1;
	}

	ftruncate(shm_fd, SIZE);

	void *ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // 映射到进程地址空间
	if (ptr == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

	sprintf((char*) ptr, "Hello from shared memory!");

	printf("Data read from shared memory: %s\n",(char*)ptr);

	munmap(ptr, SIZE); // 解除映射

	shm_unlink(name); // 移除共享内存对象

	return 0;
}
