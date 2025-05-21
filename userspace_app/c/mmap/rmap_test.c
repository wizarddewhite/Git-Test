/*
 * Fork a process tree and move the range in child
 *
 * # pstree -ap root_pid
 *
 * could show the process tree
 *
 * # pstree -A -l -p root_pid | grep -o '[0-9]\+' | while read pid; do grep -H range /proc/$pid/maps || echo "not found"; done
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
static int num_process;
static int chosen_process;
static int unmap_process = -1;

struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};
static int semid, unmap_semid;

int pipefd[2];

static char initial_data[] = "Hello, world 0!";
static char updated_data[] = "Hello, World 0!";

static unsigned int rand_seed;
static size_t mapsize;
static void *region;

#define FAIL_ON_MOVE (1)
#define FAIL_ON_CMP  (2)

static char* failure_reason[3] = {
	"Normal exit",
	"Failed to move page",
	"Failed on comparing data",
};

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

	strcpy(page, updated_data);
	return 0;
}

int is_updated(void *page)
{
	if (page == MAP_FAILED)
		return 0;

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
		printf("%d pid: %d child exit '%s'\n", num_process, getpid(), failure_reason[exit_status]);

		if (exit_status == FAIL_ON_MOVE)
			return FAIL_ON_MOVE;
	}

	return ret;
}

void init(int round)
{
	mapsize = getpagesize();

	if (numa_available() < 0) {
		printf("Numa not available, Quit\n");
		exit(EXIT_FAILURE);
	}

	if (numa_bitmask_weight(numa_all_nodes_ptr) <= 1) {
		printf("Multiple numa nodes is required, Quit\n");
		// exit(EXIT_FAILURE);
	}

	semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	unmap_semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semid == -1 || unmap_semid == -1) {
		perror("segmet failed\n");
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 0) == -1
		|| semctl(unmap_semid, 0, SETVAL, 0) == -1) {
		perror("semctl failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(pipefd) == -1) {
		perror("pipe failed\n");
		exit(EXIT_FAILURE);
	}

	rand_seed = time(NULL);
	srand(rand_seed);
	chosen_process = rand() % TOTAL_PROCESS + 1;
	/* We should ummap grand parent, so at least 3 process is required */
	if (TOTAL_PROCESS >= 3)
		unmap_process = rand() % (TOTAL_PROCESS - 2) + 1;
	/* We can't move page after unmap. So we don't unmap. */
	if (unmap_process == chosen_process)
		unmap_process = -1;
	printf("chosen_process %d, unmap_process %d\n", chosen_process, unmap_process);

	/* Map a shared area and fault in */
	region = mmap(NULL, mapsize, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
		printf("Map failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Map region: [%lx-%lx]\n", (unsigned long)region, (unsigned long)(region + mapsize));
	memset((void *)region, 1, mapsize);
	initial_data[13] = '0' + round % 10;
	updated_data[13] = '0' + round % 10;
	strcpy(region, initial_data);

	num_process = 1;
}

/* Tear down map region and semaphore. */
void cleanup()
{
	/* If we haven't unmap region, unmap it. */
	if (region != MAP_FAILED && munmap(region, mapsize)) {
		printf("Failed to unmap region\n");
		exit(EXIT_FAILURE);
	}

	region = MAP_FAILED;

	semctl(semid, 0, IPC_RMID);
	semctl(unmap_semid, 0, IPC_RMID);

	semid = unmap_semid = -1;

	close(pipefd[0]);
}

int child_process()
{
	int ret = 0;

	if (num_process == unmap_process) {
		/* This is the unmap process */
		/* Wait signal and do unmap, and then tell them we are done. */
		semop(unmap_semid, &sem_wait, 1);
		munmap(region, mapsize);
		region = MAP_FAILED;
		printf("\tummap process region unmapped...\n");
		semop(unmap_semid, &sem_signal, 1);
	}

	close(pipefd[1]);

	if (num_process == chosen_process) {
		/* This is the chosen process, first wait leaf child created */
		char buf;
		/* After all process close(pipefd[1]), it will return 0. */
		while (read(pipefd[0], &buf, 1) > 0) {
			printf("\tchild ready\n");
		}

		/* Move page here. */
		ret = try_to_move_pages(region);

		printf("chosen process %d has move page and now kick others...\n", chosen_process);
		/* kick others */
		semctl(semid, 0, IPC_RMID);
	} else {
		/*
		 * We are not the chosen_process, wait signal and check to see
		 * the update or not.
		 */
		semop(semid, &sem_wait, 1);
		ret = is_updated(region);
	}

	/*
	 * No mater what kind of process it is, we return the result of:
	 *   * try_to_move_pages(region)
	 *   * is_updated(region)
	 */
	return ret;
}

int main(int argc, char *argv[])
{
	pid_t pid;
	int ret = 0;

	for (int i = 0; i < 1; i++) {
		init(i);

		printf("%d root pid: %d\n", num_process, getpid());

		while (num_process < TOTAL_PROCESS) {
			/*
			 * Before creating the leaf process and we can do unmap,
			 * wait until unmap process did its job.
			 */
			if (num_process == TOTAL_PROCESS - 1 && unmap_process != -1) {
				semop(unmap_semid, &sem_signal, 1);
				printf("before creating leaf child\n");
				semop(unmap_semid, &sem_wait, 1);
			}

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

		ret = child_process();

		printf("%d pid: %d continue %s\n", num_process, getpid(),
			num_process == unmap_process ? "'Region Unmapped'" : (char*)region);

		ret = wait_child(ret);

		printf("%d pid: %d exit '%s'\n", num_process, getpid(), failure_reason[ret]);

		/* root process check result */
		if (num_process == 1) {
			if (!ret)
				printf("Test pass\n");
			else if (ret == FAIL_ON_MOVE)
				printf("Failed on moving page\n");
			else
				printf("One of our child doesn't see the update\n");

			printf("\n\n");
			cleanup();

			// if (ret)
			// 	break;
		} else {
			/* Only root process would go another round */
			break;
		}
	}

	return ret;
}
