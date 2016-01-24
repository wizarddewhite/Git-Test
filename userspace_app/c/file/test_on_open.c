/*
 * =====================================================================================
 *
 *       Filename:  test_on_open.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/06/2014 03:57:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#define _GNU_SOURCE
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define errExit(msg)     do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)

int main(int argc, char *argv[])
{
    struct rlimit old, new;
    struct rlimit *newp;
    pid_t pid;
    int fd, i;

    pid = getpid();

    newp = NULL;
    new.rlim_cur = 128;
    new.rlim_max = 128;
    newp = &new;

    if (prlimit(pid, RLIMIT_NOFILE, newp, &old) == -1)
        errExit("prlimit-1");
    printf("Previous limits: soft=%lld; hard=%lld\n",
            (long long) old.rlim_cur, (long long) old.rlim_max);

    getrlimit(RLIMIT_NOFILE, newp);
    printf("Current limits: soft=%lld; hard=%lld\n",
            (long long) new.rlim_cur, (long long) new.rlim_max);

    for (i = 3; i <= 128; i++) {
    	fd = open(".", O_RDONLY);
	if (fd < 0)
		perror("open");
	else
    		printf("fd:%d allocated\n", fd);
    }

    close(3);
    fd = open(".", O_RDONLY);
    if (fd < 0)
	perror("open");
    else
   	printf("fd:%d allocated\n", fd);

    exit(EXIT_FAILURE);
}


