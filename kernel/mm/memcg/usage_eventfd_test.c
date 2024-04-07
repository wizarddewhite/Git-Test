/*
 * =====================================================================================
 *
 *       Filename:  oom_eventfd_test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2022/01/08 21 ±44∑÷07√Î
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), richard.weiyang@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
// https://access.redhat.com/documentation/en-us/red_hat_enterprise_linux/6/html/resource_management_guide/sec-memory

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static inline void die(const char *msg)
{
	fprintf(stderr, "error: %s: %s(%d)\n", msg, strerror(errno), errno);
	exit(EXIT_FAILURE);
}

static inline void usage(void)
{
	fprintf(stderr, "usage: usage_eventfd_test <cgroup.event_control> <memory.usage_in_bytes>\n");
	exit(EXIT_FAILURE);
}

#define BUFSIZE 256

int main(int argc, char *argv[])
{
	char buf[BUFSIZE];
	int efd, cfd, ofd, rb, wb;
	uint64_t u;

	if (argc != 3)
		usage();

	if ((efd = eventfd(0, 0)) == -1)
		die("eventfd");

	if ((cfd = open(argv[1], O_WRONLY)) == -1)
		die("cgroup.event_control");

	if ((ofd = open(argv[2], O_RDONLY)) == -1)
		die("memory.usage_in_bytes");

	if ((wb = snprintf(buf, BUFSIZE, "%d %d 102400", efd, ofd)) >= BUFSIZE)
		die("buffer too small");

	if (write(cfd, buf, wb) == -1)
		die("write cgroup.event_control");

	if (close(cfd) == -1)
		die("close cgroup.event_control");

	for (;;) {
		if (read(efd, &u, sizeof(uint64_t)) != sizeof(uint64_t))
			die("read eventfd");

		printf("mem_cgroup usage event received\n");
	}

	return 0;
}
