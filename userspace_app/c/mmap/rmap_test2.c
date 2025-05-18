/*
 * Fork a process tree and move the range in child
 *
 * # pstree -ap root_pid
 *
 * could show the process tree
 *
 * # pstree -A -l -p root_pid | grep -o '[0-9]\+' | while read pid; do grep -H p /proc/$pid/maps || echo "not found"; done
 *
 * could show we do map/unmap region in some process as expect
 */
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <numaif.h>
#include <numa.h>

#define SIZE 1*1024*1024  // 1 MB

#define TOTAL_LEVEL 5
#define TOTAL_CHILDREN 5
static unsigned int num_process = 1;
static int num_level;
static int num_child;

static int chosen_level;
static int chosen_child;

struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};

static char initial_data[] = "Hello, world!";
static char updated_data[] = "Hello, World!";

#define FAIL_ON_MOVE (1)
#define FAIL_ON_CMP  (2)

int try_to_move_pages(void *page)
{
	int status;
	int node;
	int ret;

	ret = move_pages(0, 1, &page, NULL, &status, MPOL_MF_MOVE);

	if (ret < 0)
		return FAIL_ON_MOVE;

	printf("current node %d\n", status);

	for (node = 0; node <= numa_max_node(); node++) {
		if (numa_bitmask_isbitset(numa_all_nodes_ptr, node) && node != status) {
			printf("move to numa node %d\n", node);
			break;
		}
	}

	if (node > numa_max_node()) {
		printf("Couldn't find available numa node for testing\n");
		return FAIL_ON_MOVE;
	}

	printf("Move page %p from Node %d to Node %d\n", page, status, node);

	ret = move_pages(0, 1, &page, &node, &status, MPOL_MF_MOVE);

	printf("Page move result: %d, status is %d\n", ret, status);
	if (ret < 0)
		return FAIL_ON_MOVE;

	strcpy(page, "Hello, World!");
	return 0;
}

int is_updated(void *page)
{
	if (strncmp((char*)page, updated_data, strlen(updated_data)))
		return FAIL_ON_CMP;
	return 0;
}

int main(int argc, char *argv[])
{
	pid_t root_pid, pid;
	int semid;
	void *region;
	unsigned int rand_seed;
	int i, curr_child, curr_level = 0;
	int status = 0;
	int ret = 0;
	size_t mapsize = getpagesize();


	/* Prepare semaphore */
	semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semid == -1) {
		perror("segmet failed\n");
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 0) == -1) {
		perror("semctl failed\n");
		exit(EXIT_FAILURE);
	}

	root_pid = getpid();

	rand_seed = time(NULL);
	srand(rand_seed);
	num_level = rand() % (TOTAL_LEVEL) + 1;
	printf("%d root pid: %d, with level %d\n", num_process, root_pid, num_level);

	chosen_level = rand() % num_level;
	chosen_child = rand() % TOTAL_CHILDREN + 1;

	/* Map a shared area and fault in */
	region = mmap(0, mapsize, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	printf("Map region: [%p - %p]\n", region, region + mapsize);
	memset((void *)region, 1, mapsize);
	strcpy(region, initial_data);

repeat:
	num_child = rand_r(&rand_seed) % TOTAL_CHILDREN + 1;
	printf("level %d child %d\n", curr_level, num_child);
	for (curr_child = 0; curr_child < num_child; curr_child++) {
		pid = fork();

		if (pid < 0) {
			perror("Error: fork\n");
		} else if (pid == 0) {

			// printf("%d child %d of parent %d, %s\n", num_process, getpid(), getppid(), (char*)region);
			if (++curr_level == num_level)
				break;
			rand_seed += curr_child;
			goto repeat;
		}
	}

	sleep(13);

	/* Wait all child to quit */
	while (wait(&status) > 0);

	printf("%d quit \n", getpid());

	return ret;
}
