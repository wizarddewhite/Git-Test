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
sem_t sem_jobs;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define MAX_WORKERS 100
pthread_t worker_id[MAX_WORKERS];
unsigned int workers;
unsigned int period;
unsigned int available_workers;

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
	available_workers = workers;
	period = atoi(argv[2]);

	signal_handle_init();

	sem_init(&sem_jobs, 0, 0);

	srand(time(NULL));
}

void *worker(void *arg)
{
	pthread_t self;
	struct timeval local_time;

	self = pthread_self();

	while (1) {
		find_job();
		printf("\t---- Worker %lu invoked, (%d) left ----\n",
			self, available_workers);

		sleep(2);
		gettimeofday(&local_time, NULL);

		/* update statistics */
		jobs_delivered++;
		printf("\t---- Worker %lu done ----\n", self);

		/* Tell boss we are done */
		pthread_mutex_lock(&mut);
		if (++available_workers == workers)
			pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mut);
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

		/* Approximate random jobs */
		current_jobs = rand() % workers + 1;
		printf("Try to deliver %d jobs\n", current_jobs);

		/* Place holder for tasks */
		gettimeofday(&date, NULL);

		/* Deliver jobs */
		for (i = 0; i < current_jobs; i++) {
			available_workers--;
			deliver_job();
		}

		/* Wait until all worker finish */
		pthread_mutex_lock(&mut);
		pthread_cond_wait(&cond, &mut);
		printf("... all work done\n");
		pthread_mutex_unlock(&mut);
		sleep(period);
	}

	for (i = 0; i < workers; i++)
		pthread_join(worker_id[i], NULL);

	return 0;
}
