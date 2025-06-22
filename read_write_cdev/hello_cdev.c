#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define DEVICE_NAME "hello_cdev"

static int major;
static char text[64];

static ssize_t my_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
    int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

    pr_info("hello_cdev - Read is called, we want to read %ld bytes, but actually only copying %d bytes. The offset is %lld\n", len, to_copy, *off);

    if (*off >= sizeof(text))
        return 0;

    not_copied = copy_to_user(user_buf, &text[*off], to_copy);
    delta = to_copy - not_copied;

    if (not_copied)
        pr_warn("hello_cdev - Could only copy %d bytes\n", delta);

    *off += delta;

    return delta;
}

static ssize_t my_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
    int not_copied, delta, to_copy = (len + *off) < sizeof(text) ? len : (sizeof(text) - *off);

    pr_info("hello_cdev - Write is called, we want to read %ld bytes, but actually only copying %d bytes. The offset is %lld\n", len, to_copy, *off);

    if (*off >= sizeof(text))
        return 0;

    not_copied = copy_from_user(&text[*off], user_buf, to_copy);
    delta = to_copy - not_copied;

    if (not_copied)
        pr_warn("hello_cdev - Could only copy %d bytes\n", delta);

    *off += delta;
    return delta;
}

static int my_open(struct inode *inode, struct file *filp)
{
    pr_info("hello_cdev - Major: %d, Minor: %d\n", imajor(inode), iminor(inode));

    pr_info("hello_cdev - filp->f_pos: %lld\n", filp->f_pos);
    pr_info("hello_cdev - filp->f_mode: %x\n", filp->f_mode);
    pr_info("hello_cdev - filp->f_flags: %x\n", filp->f_flags);

    return 0;
}

static int my_release(struct inode *inode, struct file *filp)
{
    pr_info("hello_cdev - File is closed\n");

    return 0;
}

static struct file_operations fops = {
    .read = my_read,
    .open = my_open,
    .release = my_release,
    .write = my_write,
};

static int __init my_init(void)
{
    pr_info("hello_cdev - Initializing...\n");
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        pr_alert("hello_cdev - Error registering chrdev\n");
        return major;
    }

    pr_info("hello_cdev - Major Device Number: %d\n", major);

    return 0;
}

static void __exit my_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("hello_cdev - Bye :)\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gregory");
MODULE_DESCRIPTION("A sample driver for registering a character device");
