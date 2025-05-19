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
#include <numaif.h>
#include <numa.h>

#define SIZE 1*1024*1024  // 1 MB

#define TOTAL_PROCESS 4
static int num_process = 1;
static int chosen_process;

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

int wait_child(int ret)
{
	int status = 0;

	wait(&status);

	/* If child failed to move, we report this to parent. */
	if (WIFEXITED(status)) {
		int exit_status;
		exit_status = WEXITSTATUS(status);
		printf("%d pid: %d 's child exit code %d\n", num_process, getpid(), exit_status);

		if (exit_status == FAIL_ON_MOVE)
			return FAIL_ON_MOVE;
	}

	return ret;
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int semid, chosen_semid;
	void *region;
	int ret = 0;
	unsigned int rand_seed;
	size_t mapsize = getpagesize();

	if (numa_available() < 0) {
		printf("Numa not available, Quit\n");
		exit(EXIT_FAILURE);
	}

	if (numa_bitmask_weight(numa_all_nodes_ptr) <= 1) {
		printf("Multiple numa nodes is required, Quit\n");
		// exit(EXIT_FAILURE);
	}

	semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	chosen_semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semid == -1 || chosen_semid == -1) {
		perror("segmet failed\n");
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 0) == -1 || semctl(chosen_semid, 0, SETVAL, 0) == -1) {
		perror("semctl failed\n");
		exit(EXIT_FAILURE);
	}

	rand_seed = time(NULL);
	srand(rand_seed);

	printf("%d root pid: %d\n", num_process, getpid());

	/* Map a shared area and fault in */
	region = mmap(0, mapsize, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	printf("Map region: [%p - %p]\n", region, region + mapsize);
	memset((void *)region, 1, mapsize);
	strcpy(region, initial_data);

	chosen_process = rand() % TOTAL_PROCESS + 1;

	while (num_process < TOTAL_PROCESS) {
		pid = fork();

		if (pid < 0) {
			perror("Error: fork\n");
		} else if (pid == 0) {
			++num_process;
			printf("%d child %d of parent %d, %s\n", num_process, getpid(), getppid(), (char*)region);
		} else {
			break;
		}
	}

	if (num_process == TOTAL_PROCESS) {
		/* This is the leaf child */
		/* tell chosen one to start */
		printf("leaf child is running\n");
		semop(chosen_semid, &sem_signal, 1);
	}

	if (num_process == chosen_process) {
		/* This is the chosen process, first wait leaf child created */
		semop(chosen_semid, &sem_wait, 1);
		sleep(2);
		printf("chosen process %d to kick others\n", chosen_process);

		/* do some job and kick others */
		ret = try_to_move_pages(region);

		for (int i = 1; i < TOTAL_PROCESS; i++) {
			semop(semid, &sem_signal, 1);
		}
	} else {
		semop(semid, &sem_wait, 1);
		ret = is_updated(region);
	}

	printf("%d pid: %d continue %s\n", num_process, getpid(), (char*)region);

	ret = wait_child(ret);

	printf("%d pid: %d finish %d\n", num_process, getpid(), ret);

	/* root process check result */
	if (num_process == 1) {
		if (!ret)
			printf("Test pass\n");
		else if (ret == FAIL_ON_MOVE)
			printf("Failed on moving page\n");
		else
			printf("One of our child doesn't see the update\n");
	}

	return ret;
}
