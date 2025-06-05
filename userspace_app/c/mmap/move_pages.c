#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <numaif.h>
#include <numa.h>
#include <fcntl.h>
#include <sys/uio.h>

int move_to_different_node()
{
	size_t pagesize = getpagesize();
	size_t mapsize = pagesize;
	void *pages[1];
	int nodes[1], cur_node, node = -1;
	int status[1];
	int ret;

	if (numa_available() < 0) {
		printf("Numa not available, Quit\n");
		exit(-1);
	}

	printf("System page size: %lx bytes\n", pagesize);

	// map a page region
	char * region = mmap(
		NULL, // (void *)0x7f3d39000000
		mapsize,
		PROT_READ|PROT_WRITE|PROT_EXEC,
		MAP_ANON|MAP_PRIVATE,
		-1,
		0
	);
	if (region == MAP_FAILED) {
		perror("Could not mmap");
		return 1;
	}
	printf("Map to region: [%p - %p]\n", region, region + mapsize);

	// write some content to 2nd one
	strcpy(region, "Hello, world!");
	printf("Contents of region: %s \n", region);

	pages[0] = region;
	// retrieve current numa info
	ret = move_pages(0, 1, (void **)&pages, NULL, status, MPOL_MF_MOVE);

	if (!ret) {
		cur_node = status[0];
		printf("Current numa node is : %d\n", cur_node);
	}

	printf("Max numa node: %d\n", numa_max_node());
	// look for a numa node different from current one
	for (node = 0; node <= numa_max_node(); node++) {
		if (numa_bitmask_isbitset(numa_all_nodes_ptr, node) &&
			node != status[0]) {
			printf("Available numa node %d\n", node);
			break;
		}
	}

	if (node > numa_max_node()) {
		printf("Couldn't find available numa node for testing\n");
		exit(-1);
	}

	printf("Move page %p from Node %d to Node %d\n", region, cur_node, node);
	nodes[0] = node;
	ret = move_pages(0, 1, (void **)&pages, nodes, status, MPOL_MF_MOVE);

	printf("Page move result: %d, status is %d\n", ret, status[0]);

	return 0;
}

#ifndef MADV_FREE
#define MADV_FREE      8               /* free pages only if memory pressure */
#endif

#define ONE_MB         (1024 * 1024)
#define MAP_SIZE       (16 * ONE_MB)
#define THP_SIZE       (2 * ONE_MB)
#define THP_MASK       (THP_SIZE - 1)

#define ERR_EXIT_ON(cond, msg)                                 \
        do {                                                   \
                int __cond_in_macro = (cond);                  \
                if (__cond_in_macro)                           \
                        error_exit(__cond_in_macro, (msg));    \
        } while (0)

void error_msg(int ret, int nr, int *status, const char *msg)
{
        int i;

        fprintf(stderr, "Error: %s, ret : %d, error: %s\n",
                msg, ret, strerror(errno));

        if (!nr)
                return;
        fprintf(stderr, "status: ");
        for (i = 0; i < nr; i++)
                fprintf(stderr, "%d ", status[i]);
        fprintf(stderr, "\n");
}

void error_exit(int ret, const char *msg)
{
        error_msg(ret, 0, NULL, msg);
        exit(1);
}

int page_size;

bool do_vmsplice;
bool do_thp;

static int pipe_fds[2];
void *addr;
char *pn;
char *pn1;
void *pages[2];
int status[2];

void prepare()
{
        int ret;
        struct iovec iov;

        if (addr) {
                munmap(addr, MAP_SIZE);
                close(pipe_fds[0]);
                close(pipe_fds[1]);
        }

        ret = pipe(pipe_fds);
        ERR_EXIT_ON(ret, "pipe");

        addr = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        ERR_EXIT_ON(addr == MAP_FAILED, "mmap");
        if (do_thp) {
                ret = madvise(addr, MAP_SIZE, MADV_HUGEPAGE);
                ERR_EXIT_ON(ret, "advise hugepage");
        }

        pn = (char *)(((unsigned long)addr + THP_SIZE) & ~THP_MASK);
        pn1 = pn + THP_SIZE;
        pages[0] = pn;
        pages[1] = pn1;
        *pn = 1;

        if (do_vmsplice) {
                iov.iov_base = pn;
                iov.iov_len = page_size;
                ret = vmsplice(pipe_fds[1], &iov, 1, 0);
                ERR_EXIT_ON(ret < 0, "vmsplice");
        }

        status[0] = status[1] = 1024;
}

void test_migrate()
{
        int ret;
        int nodes[2] = { 1, 1 };
        pid_t pid = getpid();

        prepare();
        ret = move_pages(pid, 1, pages, nodes, status, MPOL_MF_MOVE_ALL);
        error_msg(ret, 1, status, "move 1 page");

        prepare();
        ret = move_pages(pid, 2, pages, nodes, status, MPOL_MF_MOVE_ALL);
        error_msg(ret, 2, status, "move 2 pages, page 1 not mapped");

        prepare();
        *pn1 = 1;
        ret = move_pages(pid, 2, pages, nodes, status, MPOL_MF_MOVE_ALL);
        error_msg(ret, 2, status, "move 2 pages");

        prepare();
        *pn1 = 1;
        nodes[1] = 0;
        ret = move_pages(pid, 2, pages, nodes, status, MPOL_MF_MOVE_ALL);
        error_msg(ret, 2, status, "move 2 pages, page 1 to node 0");
}

int move_and_check_status()
{
        numa_run_on_node(0);
        page_size = getpagesize();

        test_migrate();

        fprintf(stderr, "\nMake page 0 cannot be migrated:\n");
        do_vmsplice = true;
        test_migrate();

        fprintf(stderr, "\nTest THP:\n");
        do_thp = true;
        do_vmsplice = false;
        test_migrate();

        fprintf(stderr, "\nTHP: make page 0 cannot be migrated:\n");
        do_vmsplice = true;
        test_migrate();

        return 0;
}

int main(void)
{
	move_to_different_node();
	move_and_check_status();

	return 0;
}
