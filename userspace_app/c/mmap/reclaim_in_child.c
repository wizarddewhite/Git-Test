/*
 * Fork a process tree and reclaim the range in child
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

#define SIZE 128*1024*1024  // 128 MB

#define TOTAL_PROCESS 8
static int num_process = 1;

struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};

int main(int argc, char *argv[])
{
	pid_t pid;
	int semid;


	semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semid == -1) {
		perror("segmet failed\n");
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 0) == -1) {
		perror("semctl failed\n");
		exit(EXIT_FAILURE);
	}

	/* Map an area and fault in */
	char *region = mmap(0, SIZE, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	printf("Map region: [%p - %p]\n", region, region + SIZE);
	memset((void *)region, 1, SIZE);
	strcpy(region, "Hello, world!");

	printf("%d root pid: %d\n", num_process, getpid());

	while (num_process < TOTAL_PROCESS) {
		/*
		 * Now we are going to fork the last process, do two things
		 * before continue:
		 *
		 * 1. wake up all parent
		 * 2. wait a while for parent to do preparation
		 *
		 */
		if (num_process == (TOTAL_PROCESS - 1)) {
			for (int i = 1; i < TOTAL_PROCESS; i++) {
				semop(semid, &sem_signal, 1);
			}

			printf("waiting parent prepare...\n");
			sleep(5);
		}

		pid = fork();

		if (pid < 0) {
			perror("Error: fork\n");
		} else if (pid == 0) {
			++num_process;
			// printf("%d child %d of parent %d, %s\n", num_process, getpid(), getppid(), region);
		} else {
			break;
		}
	}

	/* Wait except the last process */
	if (num_process != TOTAL_PROCESS) {
		semop(semid, &sem_wait, 1);
	}
	/* For 2 and 3rd last process, we would unmap the range */
	if (num_process == 2 || num_process == (TOTAL_PROCESS - 2))
		munmap(region, SIZE);

	// printf("%d pid: %d continue\n", num_process, getpid());
	/* Wait except the last process */
	if (num_process != TOTAL_PROCESS)
		wait(NULL);
	else {
		sleep(6);
		printf("Release the page range in leaf child\n");

		/* COW to get new page */
		memset((void *)region, 2, SIZE);

		clock_t start_time = clock();
		madvise((void *)region, SIZE, MADV_PAGEOUT);
		clock_t end_time = clock();

		double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
		printf("Time taken by reclamation: %f seconds\n", elapsed_time);
	}

	// printf("%d pid: %d finish\n", num_process, getpid());
	return 0;
}
