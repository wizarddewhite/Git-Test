/*
 * =====================================================================================
 *
 *       Filename:  pthread_setspecific.c
 *       http://www.groad.net/bbs/read.php?tid-2179.html
 *       http://www.ibm.com/developerworks/cn/linux/thread/posix_threadapi/part2/
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/20/2013 08:48:39 AM
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
pthread_key_t key;
struct test_struct {
    int i;
    float k;
};

void echomsg(void* t)
{
        printf("destructor excuted in thread %lu,param=%p\n",
			pthread_self(), t);
}

void *child1 (void *arg)
{
    struct test_struct struct_data;
    struct_data.i = 10;
    struct_data.k = 3.1415;
    pthread_setspecific(key, &struct_data);
    printf ("address of struct_data 0x%p\n", &(struct_data));
    printf ("child1: pthread_getspecific(key) returns :0x%p\n", (struct test_struct *)pthread_getspecific(key));
    printf ("child1: \n\tstruct_data.i: %d\n\tstruct_data.k: %f\n", 
		    ((struct test_struct *)pthread_getspecific (key))->i, ((struct test_struct *)pthread_getspecific(key))->k);
    printf ("------------------------------------------------------\n");
    return NULL;
}
void *child2 (void *arg)
{
    int temp = 20;
    sleep (2);
    printf ("child2: address of temp 0x%p\n",  &temp);
    pthread_setspecific(key, &temp);
    printf ("child2: pthread_getspecific(key) returns :0x%p\n", (int *)pthread_getspecific(key));
    printf ("child2: \n\ttemp :%d\n", *((int *)pthread_getspecific(key)));
    return NULL;
}
int main (void)
{
    pthread_t tid1, tid2;
    pthread_key_create(&key, echomsg);
    pthread_create(&tid1, NULL, (void *)child1, NULL);
    pthread_create(&tid2, NULL, (void *)child2, NULL);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_key_delete(key);
    return (0);
}
