#include <linux/init.h>
#include <linux/module.h>
#include <linux/rwsem.h>
MODULE_LICENSE("Dual BSD/GPL");

static int readers = 0;
DECLARE_RWSEM(rw_sem_test);

void reader_start(void)
{
	down_read(&rw_sem_test);
	readers++;
}

void reader_stop(void)
{
	readers--;
	up_read(&rw_sem_test);
}

void reader_test(void)
{
	int i;

	for (i = 0; i < 10; i++)
		reader_start();

	printk(KERN_ERR "... Has %d readers now\n", readers);

	for (i = 0; i < 10; i++)
		reader_stop();

	printk(KERN_ERR "... Has %d readers now\n", readers);
}

static int rw_semaphor_init(void)
{
	reader_test();

	return 0;
}

static void rw_semaphor_exit(void)
{
	printk(KERN_ERR "Goodby, cruel world!\n");
}
module_init(rw_semaphor_init);
module_exit(rw_semaphor_exit);
