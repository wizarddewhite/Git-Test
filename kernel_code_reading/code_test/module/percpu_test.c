/*
 * =====================================================================================
 *
 *       Filename:  percpu_test.c
 *
 *    Description:  http://blog.chinaunix.net/uid-7210505-id-146389.html
 *
 *        Version:  1.0
 *        Created:  10/10/2013 10:29:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <net/inet_hashtables.h>
#include <net/protocol.h>
#include <linux/cpumask.h>

#define err(msg) printk(KERN_ALERT "%s\n", msg)

static int *percpu;

int percpu_dynamic()
{
        int *p;
        int i;

	/* Allocate percpu variable dynamicly */
        percpu = alloc_percpu(int);
        if (!percpu) {
                err("alloc_percpu");
                goto err;
        }

        for_each_possible_cpu(i) {
                p = per_cpu_ptr(percpu, i);
                *p = i;
        }

        for_each_possible_cpu(i) {
                p = per_cpu_ptr(percpu, i);
                printk(KERN_INFO "%d = %d\n", i, *p);
        }


        return 0;
err:
        return -1;
}

static int main_init(void)
{
	int ret;

	ret = percpu_dynamic();

	return ret;
}

static void main_exit(void)
{
        free_percpu(percpu);
}

module_init(main_init);
module_exit(main_exit);

MODULE_LICENSE("GPL");
