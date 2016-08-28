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
#include <semaphore.h>
#include <sys/time.h>

unsigned long jobs_delivered;
sem_t sem_jobs, sem_workers;

#define MAX_WORKERS 100
pthread_t worker_id[MAX_WORKERS];
unsigned int workers;
unsigned int period;

void jobs_statistic()
{
	printf("%lu jobs done\n", jobs_delivered);
}

void find_job()
{
	sem_wait(&sem_jobs);
}

void deliver_job()
{
	sem_post(&sem_jobs);
}

void find_worker()
{
	sem_wait(&sem_workers);
}

void return_worker()
{
	sem_post(&sem_workers);
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

void setup(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s workers period\n", argv[0]);
		exit(-1);
	}

	workers = atoi(argv[1]);
	if (workers > MAX_WORKERS)
		workers = MAX_WORKERS;
	period = atoi(argv[2]);

	signal_handle_init();

	sem_init(&sem_jobs, 0, 0);
	sem_init(&sem_workers, 0, workers);
}

void *worker(void *arg)
{
	pthread_t self;
	struct timeval local_time;

	self = pthread_self();

	while (1) {
		find_job();
		printf("\t---- Worker %lu invoked ----\n", self);

		sleep(2);
		gettimeofday(&local_time, NULL);

		jobs_delivered++;
		return_worker();
		printf("\t---- Worker %lu done ----\n", self);
	}

	return NULL;
}

int main (int argc, char *argv[])
{
	int i;

	setup(argc, argv);

	printf("Deliver jobs to %u workers every %uns\n", workers, period);

	for (i = 0; i < workers; i++)
		pthread_create(&worker_id[i], NULL, (void *)worker, NULL);

	while (1) {
		struct timeval date;
		int current_jobs;
		int waiting_workers;

		/* Approximate random jobs */
		current_jobs = time(NULL) % workers;
		printf("Try to deliver %d jobs\n", current_jobs);

		/* Place holder for tasks */
		gettimeofday(&date, NULL);

		/* Deliver jobs */
		for (i = 0; i < current_jobs; i++) {
			find_worker();
			deliver_job();
		}

		waiting_workers = 0;
		while (waiting_workers != workers)
			sem_getvalue(&sem_workers, &waiting_workers);
	}

	for (i = 0; i < workers; i++)
		pthread_join(worker_id[i], NULL);

	return 0;
}
