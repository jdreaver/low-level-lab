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

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);

static struct task_struct *thread;

static int thread_fn(void *data)
{
	while (1) {
		pr_info("in kernel thread with PID: %d\n", current->pid);

		if (wait_event_interruptible(wee_wait, kthread_should_stop()))
			return -ERESTARTSYS;

		if (kthread_should_stop())
			break;
	}

	return 0;
}

static char *message = "7c1caf2f50d1";
#define message_len 13 // Length of message + null byte

static ssize_t misc_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	char copied[message_len];

	pr_info("in %s misc_write. count: %zu\n", THIS_MODULE->name, count);

	// We can't just strncmp(message, buf, count) because the passed in
	// buffer is a userspace pointer. We first have to copy it to kernel
	// space.
	if ((simple_write_to_buffer(copied, count, ppos, buf, message_len) < 0)) {
		pr_crit("simple_write_to_buffer failed!\n");
		return -EINVAL;
	}
	copied[message_len - 1] = '\0';

	pr_info("copied message: %s, strncmp() = %d\n", copied, strncmp(message, copied, message_len));
	if (strncmp(message, copied, message_len) == 0) {
		return message_len - 1;
	}
	return -EINVAL;
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
	pr_info("Unloaded %s.\n", THIS_MODULE->name);
	misc_deregister(&misc);
	kthread_stop(thread);
}

module_init(misc_start);
module_exit(misc_end);
