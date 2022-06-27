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

static ssize_t misc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	pr_info("in %s misc_read", THIS_MODULE->name);
	return simple_write_to_buffer(buf, count, f_pos, message, strlen(message));
}

static ssize_t misc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	if (strncmp(message, buf, count) == 0) {
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
