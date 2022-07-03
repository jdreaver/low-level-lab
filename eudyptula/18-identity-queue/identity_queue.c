#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/wait.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("Eudyptula task 17 wait queue");
MODULE_VERSION("0.1");

#define NAME_LEN 20

struct identity {
	char name[NAME_LEN];
	int id;
	bool busy;
	struct list_head list;
};

static int next_id = 0;

static LIST_HEAD(identity_list);
static DEFINE_MUTEX(identity_mutex);

static int identity_create(char *name, int id)
{
	struct identity *new_ident;

	// TODO: Look for id in existing list and fail if exists

	new_ident = kmalloc(sizeof(*new_ident), GFP_KERNEL);
	if (new_ident == NULL) {
		return -EINVAL;
	}

	strscpy(new_ident->name, name, NAME_LEN);
	new_ident->id = id;
	new_ident->busy = false;

	if (mutex_lock_interruptible(&identity_mutex))
		return -EINTR;
	list_add(&(new_ident->list), &identity_list);
	mutex_unlock(&identity_mutex);

	return 0;
}

static struct identity *identity_get(void)
{
	struct identity *next_ident;
	if (list_empty(&identity_list))
		return NULL;

	if (mutex_lock_interruptible(&identity_mutex))
		return NULL;

	next_ident = list_first_entry(&identity_list, struct identity, list);
	list_del(&(next_ident->list));

	mutex_unlock(&identity_mutex);
	return next_ident;
}

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);

static struct task_struct *thread;

static int thread_fn(void *data)
{
	struct identity *next_ident;
	while (1) {
		pr_info("in kernel thread with PID: %d\n", current->pid);

		if (!wait_event_interruptible(wee_wait, (next_ident = identity_get()) || kthread_should_stop())) {
			if (next_ident) {
				pr_info("Found ident %s with id %d\n", next_ident->name, next_ident->id);
				kfree(next_ident);
				next_ident = NULL;
				msleep_interruptible(5000);
			}
		} else {
			return -ERESTARTSYS;
		}

		if (kthread_should_stop())
			break;
	}

	return 0;
}

static ssize_t misc_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	char name[NAME_LEN];

	pr_info("in %s misc_write. count: %zu\n", THIS_MODULE->name, count);

	if ((simple_write_to_buffer(name, NAME_LEN, ppos, buf, count) < 0)) {
		pr_crit("simple_write_to_buffer failed!\n");
		return -EINVAL;
	}
	name[NAME_LEN - 1] = '\0';

	if (identity_create(name, next_id) < 0)
		return -ENOMEM;

	next_id++;
	wake_up_interruptible(&wee_wait);
	return count;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.write = misc_write,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &fops,
	.mode = 0222, // -w--w--w-
};

static int __init misc_start(void)
{
	pr_info("Loading %s. Minor number is %d\n", THIS_MODULE->name, misc.minor);

	thread = kthread_run(thread_fn, NULL, "eudyptula");
	if (!thread) {
		pr_crit("failed to create thread");
	}

	return misc_register(&misc);
}

static void __exit misc_end(void)
{
	struct identity *next_ident;

	pr_info("Unloading %s.\n", THIS_MODULE->name);
	misc_deregister(&misc);
	kthread_stop(thread);

	// Clean up identities
	while ((next_ident = identity_get())) {
		pr_info("Freeing ident %s with id %d\n", next_ident->name, next_ident->id);
		kfree(next_ident);
	}
}

module_init(misc_start);
module_exit(misc_end);
