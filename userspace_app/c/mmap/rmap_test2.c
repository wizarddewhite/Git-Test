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
#include <stdbool.h>
#include <numaif.h>
#include <numa.h>

int test_verbose;
#define printv(verbosity_level, fmt, ...) \
	if(test_verbose >= verbosity_level) \
		printf(fmt, ##__VA_ARGS__)

#define SIZE 1*1024*1024  // 1 MB

#define TOTAL_LEVEL 5
#define TOTAL_CHILDREN 3
static unsigned int num_level;
static unsigned int num_child;

static unsigned int worker_level;
static unsigned int worker_child;

struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};
static int semid;

static int pipefd[2];

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

struct process_state {
	bool	is_worker;
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
	if (status < 0)
		return FAIL_ON_MOVE;

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

void init()
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

	if (pipe(pipefd) == -1) {
		perror("pipe failed\n");
		exit(EXIT_FAILURE);
	}

	rand_seed = time(NULL);
	srand(rand_seed);

	num_level = rand() % (TOTAL_LEVEL) + 1;
	worker_level = rand() % num_level + 1;

	if (num_level <= 1) {
		printf("A process tree with more than 1 level is required\n");
		num_level = 2;
	}

	printf("Expect to create tree with %d levels and worker at %d level\n",
			num_level, worker_level);

	/* Map a shared area and fault in */
	region = mmap(0, mapsize, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
		printf("Map failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Map region: [%lx-%lx]\n", (unsigned long)region, (unsigned long)(region + mapsize));
	memset((void *)region, 1, mapsize);
	strcpy(region, initial_data);
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
	semid = -1;

	close(pipefd[0]);
}

int child_process(struct process_state *state)
{
	int ret = 0;

	close(pipefd[1]);

	if (state->is_worker) {
		/* This is the worker process, first wait last process created */
		char buf;
		while (read(pipefd[0], &buf, 1) > 0) ;

		ret = try_to_move_pages(region);

		printf("worker %d has done its job and kick others...\n", getpid());
		/* kick others */
		semctl(semid, 0, IPC_RMID);
	} else {
		/* Wait... */
		semop(semid, &sem_wait, 1);
		ret = is_updated(region);
	}

	return ret;
}

int main(int argc, char *argv[])
{
	pid_t root_pid, pid;
	int opt;
	int curr_child, curr_level = 1;
	int status = 0;
	int ret = 0;
	struct process_state state = {
		.is_worker = true,
	};

	while ((opt = getopt(argc, argv, "v")) != -1) {
		if (opt == 'v')
			test_verbose++;
	}

	init();

	root_pid = getpid();
	printf("root pid: %d, with level %d\n", root_pid, num_level);

repeat:
	num_child = rand_r(&rand_seed) % TOTAL_CHILDREN + 1;
	worker_child = rand_r(&rand_seed) % num_child;
	printv(1, "propagate %d's level %d child %d worker_child %d\n",
			getpid(), curr_level + 1, num_child, worker_child);
	for (curr_child = 0; curr_child < num_child; curr_child++) {
		pid = fork();

		if (pid < 0) {
			perror("Error: fork\n");
		} else if (pid == 0) {
			curr_level++;

			if (state.is_worker && curr_child == worker_child
				&& curr_level <= worker_level)
				state.is_worker = true;
			else
				state.is_worker = false;

			printv(2, "  level %d %schild %d of parent %d, %s\n",
				curr_level, state.is_worker ? "worker " : "",
				getpid(), getppid(), (char*)region);

			if (curr_level == num_level)
				break;

			rand_seed += curr_child;
			goto repeat;
		} else if (curr_child == num_child - 1) {
			if (curr_level < worker_level)
				state.is_worker = false;
		}
	}

	ret = child_process(&state);
	printv(2, "pid: %d continue %s\n", getpid(), (char*)region);
	
	/* Wait all child to quit */
	while (wait(&status) > 0) {
		/* If child failed to move, we report this to parent. */
		if (WIFEXITED(status)) {
			int exit_status;
			exit_status = WEXITSTATUS(status);
			printv(1, "pid: %d child exit '%s'\n", getpid(), failure_reason[exit_status]);

			if (exit_status == FAIL_ON_MOVE)
				ret = FAIL_ON_MOVE;
		}
	}

	printv(2, "%d quit \n", getpid());

	if (getpid() == root_pid) {
		if (!ret)
			printf("Test pass\n");
		else if (ret == FAIL_ON_MOVE)
			printf("Failed on moving page\n");
		else
			printf("One of our child doesn't see the update\n");

		printf("\n\n");
		cleanup();
	}

	return ret;
}
