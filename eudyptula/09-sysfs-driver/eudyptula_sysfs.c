#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Reaver");
MODULE_DESCRIPTION("sysfs device driver for eudyptula task 8");
MODULE_VERSION("0.1");

static char *message = "7c1caf2f50d1";
#define message_len 13 // Length of message + null byte

static struct kobject *eudyptula_kobj;

static char foo_buf[PAGE_SIZE];
static DEFINE_SEMAPHORE(foo_sem);

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("in %s id_show", THIS_MODULE->name);
	return sysfs_emit(buf, "%s\n", message);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("in %s id_write. count: %zu\n", THIS_MODULE->name, count);
	if (strncmp(message, buf, message_len) == 0) {
		return message_len - 1;
	}
	return -EINVAL;
}

static struct kobj_attribute id_attribute =
	__ATTR(id, 0664, id_show, id_store);


static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	pr_info("in %s jiffies_read, jiffies = %lu\n", THIS_MODULE->name, jiffies);
	return sysfs_emit(buf, "%lu\n", jiffies);
}

static struct kobj_attribute jiffies_attribute =
	__ATTR(jiffies, 0444, jiffies_show, NULL);


static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	size_t len;

	pr_info("in %s foo_read", THIS_MODULE->name);

	down(&foo_sem);

	len = strlen(foo_buf);
	strncpy(buf, foo_buf, len);

	up(&foo_sem);

	return len;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	pr_info("in %s foo_write. count: %zu\n", THIS_MODULE->name, count);

	down(&foo_sem);

	strncpy(foo_buf, buf, count);

	up(&foo_sem);

	return count;
}

static struct kobj_attribute foo_attribute =
	__ATTR(foo, 0664, foo_show, foo_store);


static struct attribute *attrs[] = {
	&id_attribute.attr,
	&jiffies_attribute.attr,
	&foo_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init eudyptula_sysfs_start(void)
{
	int retval;

	pr_info("Loading module %s\n", THIS_MODULE->name);
	eudyptula_kobj = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!eudyptula_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(eudyptula_kobj, &attr_group);
	if (retval) {
		pr_crit("%s: failed to create kobj group", THIS_MODULE->name);
		goto r_id;
	}

	return 0;

r_id:
	kobject_put(eudyptula_kobj);

	return -1;
}

static void __exit eudyptula_sysfs_end(void)
{
	pr_info("Unloaded eudyptula module.\n");
	kobject_put(eudyptula_kobj);
}

module_init(eudyptula_sysfs_start);
module_exit(eudyptula_sysfs_end);
