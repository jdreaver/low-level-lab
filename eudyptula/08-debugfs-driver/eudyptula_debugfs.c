#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/semaphore.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("debugfs device driver for eudyptula task 8");
MODULE_VERSION("0.1");

static char *message = "7c1caf2f50d1";
#define message_len 13 // Length of message + null byte

static struct dentry *eudyptula_dir;
static struct dentry *eudyptula_id_file;
static struct dentry *jiffies_file;
static struct dentry *foo_file;

#define JIFFIES_BUF_SIZE 256
static char jiffies_buf[JIFFIES_BUF_SIZE];

static char foo_buf[PAGE_SIZE];
static DEFINE_SEMAPHORE(foo_sem);

static ssize_t eudyptula_id_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	pr_info("in %s eudyptula_id_read", THIS_MODULE->name);
	return simple_read_from_buffer(buf, count, ppos, message, message_len);
}

static ssize_t eudyptula_id_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	char copied[message_len];

	pr_info("in %s eudyptula_id_write. count: %zu\n", THIS_MODULE->name, count);

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

static struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = eudyptula_id_read,
	.write = eudyptula_id_write,
};

static ssize_t jiffies_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	pr_info("in %s jiffies_read, jiffies = %lu\n", THIS_MODULE->name, jiffies);
	snprintf(jiffies_buf, JIFFIES_BUF_SIZE, "%lu", jiffies);
	return simple_read_from_buffer(buf, count, ppos, jiffies_buf, JIFFIES_BUF_SIZE);
}

static struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read,
};

static ssize_t eudyptula_foo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	int retval = -EINVAL;

	pr_info("in %s eudyptula_foo_read", THIS_MODULE->name);

	down(&foo_sem);

	retval = simple_read_from_buffer(buf, count, ppos, foo_buf, PAGE_SIZE);
	if (retval < 0)
		retval = -EINVAL;

	up(&foo_sem);

	return retval;
}

static ssize_t eudyptula_foo_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	int retval = -EINVAL;

	pr_info("in %s eudyptula_foo_write. count: %zu\n", THIS_MODULE->name, count);

	down(&foo_sem);

	// TODO: Check if write bigger than PAGE_SIZE?
	// TODO: Do we need to write the whole page?
	retval = simple_write_to_buffer(foo_buf, count, ppos, buf, PAGE_SIZE);
	if (retval < 0)
		retval = -EINVAL;

	up(&foo_sem);

	return retval;
}

static struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = eudyptula_foo_read,
	.write = eudyptula_foo_write,
};

static int __init eudyptula_debugfs_start(void)
{
	pr_info("Loading module %s\n", THIS_MODULE->name);
	eudyptula_dir = debugfs_create_dir("eudyptula", NULL);

	eudyptula_id_file = debugfs_create_file("id", 0666, eudyptula_dir, NULL, &id_fops);
	jiffies_file = debugfs_create_file("jiffies", 0444, eudyptula_dir, NULL, &jiffies_fops);
	foo_file = debugfs_create_file("foo", 0444, eudyptula_dir, NULL, &foo_fops);

	return 0;
}

static void __exit eudyptula_debugfs_end(void)
{
	pr_info("Unloaded eudyptula module.\n");
	debugfs_remove(foo_file);
	debugfs_remove(jiffies_file);
	debugfs_remove(eudyptula_id_file);
	debugfs_remove(eudyptula_dir);
}

module_init(eudyptula_debugfs_start);
module_exit(eudyptula_debugfs_end);
