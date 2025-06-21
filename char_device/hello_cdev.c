#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define DEVICE_NAME "hello_cdev"

static int major;

static ssize_t my_read(struct file *f, char __user *u, size_t l, loff_t *o)
{
    printk(KERN_INFO "hello_cdev - Read is called\n");
    return 0;
}

static struct file_operations fops = {
    .read = my_read
};

static int __init my_init(void)
{
    printk(KERN_INFO "hello_cdev - Initializing...\n");
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        printk(KERN_ALERT "hello_cdev - Error registering chrdev\n");
        return major;
    }

    printk(KERN_INFO "hello_cdev - Major Device Number: %d\n", major);

    return 0;
}

static void __exit my_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "hello_cdev - Bye :)\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gregory");
MODULE_DESCRIPTION("A sample driver for registering a character device");
