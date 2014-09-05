/*
 * Copyied from:
 *  https://www.ibm.com/developerworks/linux/library/l-tasklets/
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static struct workqueue_struct *my_wq;

typedef struct {
  struct work_struct my_work;
  int    x;
} my_work_t;

my_work_t *work, *work2;

typedef struct {
	struct delayed_work   delay_work;
	int                   x;
}my_delay_work_t;

my_delay_work_t *work_d;


static void my_wq_function( struct work_struct *work)
{
	my_work_t *my_work;

	my_work = container_of(work, my_work_t, my_work);

	printk( "my_work.x %d\n", my_work->x );

	kfree(my_work);

	return;
}

static void my_delay_wq_function(struct work_struct *work)
{
	my_delay_work_t *wk;
	static int i = 0;

	wk = container_of(work, my_delay_work_t, delay_work.work);

	printk("my_delay_work.x %d\n", wk->x);

	if (i < 5) {
		queue_delayed_work(my_wq, &work_d->delay_work, 2*HZ);
		i++;
	}
	else
		kfree(wk);

	return;
}

int workqueue_init( void )
{
	int ret;

	printk(KERN_ERR "workqueue test\n");
	/* By running 'ps -ef', it's shown with name "my_queue" */
	my_wq = create_workqueue("my_queue");
	if (my_wq) {
		/* Queue some work (item 1) */
		work = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		if (work) {
			INIT_WORK(&work->my_work, my_wq_function );
			work->x = 1;

			ret = queue_work(my_wq, &work->my_work);
		}

		/* Queue some additional work (item 2) */
		work2 = (my_work_t *)kmalloc(sizeof(my_work_t), GFP_KERNEL);
		if (work2) {
			INIT_WORK(&work2->my_work, my_wq_function );
			work2->x = 2;

			ret = queue_work(my_wq, &work2->my_work);
		}

		/* Queue a delayed work */
		work_d = (my_delay_work_t *)kmalloc(sizeof(my_delay_work_t), GFP_KERNEL);
		if (work_d) {
			INIT_DELAYED_WORK(&work_d->delay_work, my_delay_wq_function);
			work_d->x = 3;
			ret = queue_delayed_work(my_wq, &work_d->delay_work, 2*HZ);
		}
	}


	return 0;
}

void workqueue_exit( void )
{
	flush_workqueue( my_wq );
	destroy_workqueue( my_wq );
	return;
}

module_init(workqueue_init);
module_exit(workqueue_exit);
