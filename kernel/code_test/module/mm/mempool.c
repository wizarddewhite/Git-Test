/*
 * =====================================================================================
 *
 *       Filename:  mempool.c
 *
 *    Description:  http://lwn.net/Articles/185078/
 *         mempool_t *mempool_create_page_pool(int min_nr, int order);
 *         mempool_t *mempool_create_kmalloc_pool(int min_nr, size_t size);
 *         mempool_t *mempool_create_kzalloc_pool(int min_nr, size_t size);
 *         mempool_t *mempool_create_slab_pool(int min_nr, 
 *                                       struct kmem_cache *cache);
 *
 *        Version:  1.0
 *        Created:  11/13/2014 09:54:46 AM
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
#include <linux/mempool.h>
MODULE_LICENSE("Dual BSD/GPL");

mempool_t *test_pool;

static int mempool_test_init(void)
{
	void *p;

	/* Create a mempool with 10 block, each block is 512bytes */
	test_pool = mempool_create_kmalloc_pool(10, 512);
	if (!test_pool)
		return -ENOMEM;

	p = mempool_alloc(test_pool, GFP_KERNEL);
	mempool_free(p, test_pool);

	return 0;
}

static void mempool_test_exit(void)
{
	mempool_destroy(test_pool);
}

module_init(mempool_test_init);
module_exit(mempool_test_exit);
