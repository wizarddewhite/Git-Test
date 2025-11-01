#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/user.h>

#define NS_PER_SEC 1000000000LL

struct thread_info {
	pthread_t thread_id;
	long long duration;
};

static int thread_cnt = 100;
static int vma_cnt = 2;
static int page_cnt = 10;

static void *thread_main(void *param) {
	struct thread_info *tinfo = (struct thread_info*)param;
	struct timespec start, end;
	int vma;

	/* Record the start time */
	if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
		perror("clock_gettime failed\n");
		return NULL;
	}

	for (vma = 0; vma < vma_cnt; vma++) {
		int page;

		/* Allocate pages */
		char *ptr = mmap(NULL, page_cnt * PAGE_SIZE,
				 PROT_READ | PROT_WRITE,
				 MAP_PRIVATE | MAP_ANON, 0, 0);
		if (ptr == MAP_FAILED) {
			perror("mmap failed\n");
			return NULL;
		}

		/* Fault the pages in */
		for (page = 0; page < page_cnt; page++, ptr += PAGE_SIZE)
			*ptr = (char)page;
	}

	/* Record the end time */
	if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
		perror("clock_gettime failed\n");
		return NULL;
	}

	tinfo->duration = (long long)(end.tv_sec - start.tv_sec) * NS_PER_SEC +
			(end.tv_nsec - start.tv_nsec);

	return NULL;
}

int main(int argc, char* argv[]) {
	struct thread_info *tinfo;
	long long total_time = 0;
	int child;

	/* Params: thread cound, mmap count, page count */
	if (argc > 1)
		thread_cnt = atoi(argv[1]);
	if (argc > 2)
		vma_cnt = atoi(argv[2]);
	if (argc > 3)
		page_cnt = atoi(argv[3]);

	printf("Spawning %d threads each allocating %d vmas of %d pages!\n",
			thread_cnt, vma_cnt, page_cnt);

	tinfo = malloc(sizeof(struct thread_info) * thread_cnt);
	if (!tinfo) {
		perror("malloc failed\n");
		return 1;
	}

	/* Create threads */
	for (child = 0; child < thread_cnt; child++) {
		if (pthread_create(&tinfo[child].thread_id, NULL, thread_main,
				   &tinfo[child])) {
			perror("pthread_create failed\n");
			return 1;
		}
	}

	/* Wait for threads to finish */
	for (child = 0; child < thread_cnt; child++) {
		void *ret;
		if (pthread_join(tinfo[child].thread_id, &ret)) {
			perror("pthread_join failed\n");
			return 1;
		}
	}

	/* Collect and report the stats */
	for (child = 0; child < thread_cnt; child++)
		total_time += tinfo[child].duration;
	printf("Total:\t%lldns\n", total_time);
	printf("Avg:\t%lldns\n", total_time / thread_cnt);

	free(tinfo);

	return 0;
}
