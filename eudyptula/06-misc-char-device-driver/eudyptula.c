#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("Simple misc char device driver for eudyptula task 6");
MODULE_VERSION("0.1");

static char *message = "7c1caf2f50d1";

static ssize_t misc_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	pr_info("in %s misc_read", THIS_MODULE->name);
	return simple_read_from_buffer(buf, count, ppos, message, strlen(message));
}

static ssize_t misc_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	pr_info("in %s misc_write. count: %zu\n", THIS_MODULE->name, count);

	// We can't just strncmp(message, buf, count) because the passed in
	// buffer is a userspace pointer. We first have to copy it to kernel
	// space.
	char copied[count + 1];
	if ((simple_write_to_buffer(copied, count, ppos, buf, count) < 0)) {
		pr_crit("simple_write_to_buffer failed!\n");
		return -EINVAL;
	}
	copied[count] = '\0';

	pr_info("copied message: %s, strncmp() = %d\n", copied, strncmp(message, copied, strlen(message)));
	if (strncmp(message, copied, strlen(message)) == 0) {
		return strlen(message); // TODO: Incorrect?
	}
	return -EINVAL;
}

static struct file_operations eudyptula_fops = {
	.owner = THIS_MODULE,
	.read = misc_read,
	.write = misc_write,
};

static struct miscdevice eudyptula_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "eudyptula",
	.fops = &eudyptula_fops,
};

static int __init misc_start(void)
{
	pr_info("Loading %s module. Minor number is %d\n", THIS_MODULE->name, eudyptula_misc.minor);
	return misc_register(&eudyptula_misc);
}

static void __exit misc_end(void)
{
	pr_info("Unloaded eudyptula module.\n");
	misc_deregister(&eudyptula_misc);
}

module_init(misc_start);
module_exit(misc_end);
