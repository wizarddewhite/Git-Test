/*
 * =====================================================================================
 *
 *       Filename:  worker_sync.c
 *
 *    Description:  This is a demo program to mimic a boss deliver jobs to
 *                  several workers. 
 *                  The boss will wake up workers and deliver jobs to them
 *                  periodically.
 *
 *        Version:  1.0
 *        Created:  08/28/2016 09:37:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

unsigned long jobs_delivered;

#define MAX_WORKERS 100
pthread_t worker_id[MAX_WORKERS];
unsigned int workers;
unsigned int period;

void jobs_statistic()
{
	printf("%lu jobs delivered\n", jobs_delivered);
}

void signal_handle_init()
{
	struct sigaction new_handler;
	sigset_t blocked;

	new_handler.sa_handler = jobs_statistic;
	new_handler.sa_flags   = SA_RESTART | SA_NODEFER;

	sigemptyset(&blocked);
	sigaddset(&blocked, SIGQUIT);
	sigaddset(&blocked, SIGTSTP);
	new_handler.sa_mask = blocked;

	if (sigaction(SIGINT, &new_handler, NULL) == -1) {
		perror("sigaction");
		exit(-1);
	}
}

void *worker(void *arg)
{
	pthread_t self;
	self = pthread_self();
	printf("---- Worker %lu running ----\n", self);

	return NULL;
}

int main (int argc, char *argv[])
{
	int i;

	if (argc != 3) {
		printf("Usage: %s workers period\n", argv[0]);
		exit(-1);
	}

	signal_handle_init();

	workers = atoi(argv[1]);
	if (workers > MAX_WORKERS)
		workers = MAX_WORKERS;
	period = atoi(argv[2]);
	printf("Deliver jobs to %u workers every %uns\n", workers, period);

	for (i = 0; i < workers; i++)
		pthread_create(&worker_id[i], NULL, (void *)worker, NULL);

	for (i = 0; i < workers; i++)
		pthread_join(worker_id[i], NULL);

	return 0;
}
