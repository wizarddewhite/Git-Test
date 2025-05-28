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
static unsigned int unmap_level = -1;
static unsigned int unmap_child;

struct sembuf sem_wait = {0, -1, 0};
struct sembuf sem_signal = {0, 1, 0};
static int semid, unmap_semid;

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
	bool	is_unmap;
	int	curr_level;
};

int try_to_move_pages()
{
	int ret;
	int node;
	int status = 0;
	int repeats = 10;
	char __attribute__((unused)) c;

repeat:
	c = *(char*)region;
	ret = move_pages(0, 1, (void **)&region, NULL, &status, MPOL_MF_MOVE_ALL);

	if (ret != 0) {
		printf("worker %d move_pages ret %d:%d\n", getpid(), ret, status);
		perror("move pages");
		return FAIL_ON_MOVE;
	}

	printv(1, "worker %d move_pages ret %d on node %d: %s\n",
		getpid(), ret, status, (char *)region);

	if (status < 0) {
		if (repeats--) {
			goto repeat;
		}
		printf("move page failed too many times\n");
		return FAIL_ON_MOVE;
	}

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

	printv(1, "Move region %p from Node %d to Node %d\n", region, status, node);

	ret = move_pages(0, 1, (void **)region, &node, &status, MPOL_MF_MOVE_ALL);

	printv(1, "Page move result: %d, status is %d\n", ret, status);
	if (ret != 0)
		return FAIL_ON_MOVE;

	strcpy(region, updated_data);
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
	unmap_semid = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
	if (semid == -1 || unmap_semid == -1) {
		perror("segmet failed\n");
		exit(EXIT_FAILURE);
	}

	if (semctl(semid, 0, SETVAL, 0) == -1 ||
		semctl(unmap_semid, 0, SETVAL, 0) == -1) {
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
	/* We should ummap grand parent, so at least 3 process is required */
	if (num_level >= 3)
		unmap_level = rand() % (num_level - 2) + 1;

	if (num_level <= 1) {
		printf("A process tree with more than 1 level is required\n");
		num_level = 2;
	}

	printv(1, "Expect to create tree with %d levels and worker at %d level, unmap at %d level\n",
			num_level, worker_level, unmap_level);

	/* Map a shared area and fault in */
	region = mmap(0, mapsize, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (region == MAP_FAILED) {
		printf("Map failed\n");
		exit(EXIT_FAILURE);
	}
	printv(1, "Map region: [%lx-%lx]\n", (unsigned long)region, (unsigned long)(region + mapsize));
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
	semctl(unmap_semid, 0, IPC_RMID);
	semid = unmap_semid = -1;

	close(pipefd[0]);
}

int child_process(struct process_state *state)
{
	int ret = 0;

	close(pipefd[1]);

	if (state->is_unmap) {
		printv(1, "unmap process %d \n", getpid());
	}

	if (state->is_worker) {
		/* This is the worker process, first wait last process created */
		char buf;
		while (read(pipefd[0], &buf, 1) > 0) ;

		ret = try_to_move_pages();

		printv(1, "worker %d has done its job and kick others...\n", getpid());
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
	int curr_child;
	int status = 0;
	int ret = 0;
	struct process_state state = {
		.is_worker = true,
		.curr_level = 1,
	};

	while ((opt = getopt(argc, argv, "v")) != -1) {
		if (opt == 'v')
			test_verbose++;
	}

	init();
	state.is_unmap = (unmap_level != -1);

	root_pid = getpid();
	printv(1, "root pid: %d, with level %d is%s unmap\n",
		root_pid, num_level, state.is_unmap ? "":"n't");

repeat:
	num_child = rand_r(&rand_seed) % TOTAL_CHILDREN + 1;
	worker_child = state.is_worker ? rand_r(&rand_seed) % num_child : -1;
	unmap_child = state.is_unmap ? rand_r(&rand_seed) % num_child : -1;
	/* We can't move page after unmap */
	if (worker_level == unmap_level && worker_child == unmap_child) {
		unmap_child = -1;
	}
	printv(2, "propagate %d's level %d child %d worker_child %d unmap_child %d\n",
			getpid(), state.curr_level + 1, num_child, worker_child, unmap_child);
	for (curr_child = 0; curr_child < num_child; curr_child++) {
		/*
		 * Before creating the leaf process, we should do unmap.
		 * Wait until unmap process did its job.
		 */
		if (state.curr_level == num_level - 1 && unmap_level != -1) {
			// printf("before creating leaf child\n");
			// semop(unmap_semid, &sem_wait, 1);
		}

		pid = fork();

		if (pid < 0) {
			perror("Error: fork\n");
		} else if (pid == 0) {
			state.curr_level++;

			if (curr_child == worker_child && state.curr_level <= worker_level)
				state.is_worker = true;
			else
				state.is_worker = false;

			if (curr_child == unmap_child && state.curr_level <= unmap_level)
				state.is_unmap = true;
			else
				state.is_unmap = false;

			printv(2, "  level %d %d of parent %d, %s is %s %s\n",
				state.curr_level, getpid(), getppid(), (char*)region,
				state.is_worker ? "worker " : "",
				state.is_unmap ? "unmap " : "");

			if (state.curr_level == num_level)
				break;

			rand_seed += curr_child;
			goto repeat;
		} else if (curr_child == num_child - 1) {
			if (state.curr_level < worker_level) {
				state.is_worker = false;
			}
			if (state.curr_level < unmap_level) {
				state.is_unmap = false;
			}
		}
	}

	ret = child_process(&state);
	printv(2, "  pid %d continue %s\n", getpid(), (char*)region);
	
	/* Wait all child to quit */
	while (wait(&status) > 0) {
		/* If child failed to move, we report this to parent. */
		if (WIFEXITED(status)) {
			int exit_status;
			exit_status = WEXITSTATUS(status);
			printv(2, "    pid: %d child exit '%s'\n", getpid(), failure_reason[exit_status]);

			if (exit_status == FAIL_ON_MOVE)
				ret = FAIL_ON_MOVE;
		}
	}

	printv(2, "  %d quit \n", getpid());

	/* root process handle the final result */
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
