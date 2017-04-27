/*
 * =====================================================================================
 *
 *       Filename:  kmem_cache.c
 *
 *    Description:  /proc/slabinfo
 *
 *        Version:  1.0
 *        Created:  04/26/2014 09:12:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Wei Yang (weiyang), weiyang.kernel@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
MODULE_LICENSE("Dual BSD/GPL");

#define ALLOC_NUM 32

static struct kmem_cache *test_cache;
void* tmp[ALLOC_NUM];

static void init_once(void *foo)
{
	static int num;
	printk(KERN_ERR "%s: [%02d]%p is created\n", __func__, num++, foo);
}

static int kmem_cache_test_init(void)
{
	int i;
	test_cache = kmem_cache_create("test_cache", 1234, 4,
			0, init_once);
	if (test_cache == NULL)
		return -ENOMEM;

	printk(KERN_ERR "%s: Fetch a cache from kmem_cache\n", __func__);
	for (i = 0; i < ALLOC_NUM; i++)
		tmp[i] = kmem_cache_zalloc(test_cache, GFP_KERNEL);

	return 0;
}
static void kmem_cache_test_exit(void)
{
	int i;
	for (i = 0; i < ALLOC_NUM; i++)
		kmem_cache_free(test_cache, tmp[i]);
	kmem_cache_destroy(test_cache);
}
module_init(kmem_cache_test_init);
module_exit(kmem_cache_test_exit);
