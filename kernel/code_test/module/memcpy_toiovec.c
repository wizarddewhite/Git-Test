/*
 * =====================================================================================
 *
 *       Filename:  memcpy_toiovec.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/30/2013 09:30:08 AM
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
MODULE_LICENSE("Dual BSD/GPL");

/* a simplified version of skb_frag_t */
struct frag_struct {
	char data[128];
	__u32 offset;
	__u32 size;
};

struct frag_struct frag_data[10];
struct iovec msg_iov[12];

void frag_data_init()
{
}

static int memcpy_toiovec_init(void)
{
	printk(KERN_ALERT "Hello, world\n");

	return 0;
}
static void memcpy_toiovec_exit(void)
{
        printk(KERN_ALERT "Goodbye, cruel world\n");
}
module_init(memcpy_toiovec_init);
module_exit(memcpy_toiovec_exit);
