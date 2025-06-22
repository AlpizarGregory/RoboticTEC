#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>


#define DEVICE_NAME "custom_driver"
#define CLASS_NAME "custom_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gregory & Cristhofer");
MODULE_DESCRIPTION("Custom Character Device Driver for Arduino Communication");

static int major;
static struct class* arduino_class = NULL;
static struct device* arduino_device = NULL;

static char message[256] = {0};
static short message_len;
static int open_count = 0;

static int my_open(struct inode *inodep, struct file *filep)
{
    open_count++;

    pr_info("%s - Major: %d, Minor: %d\n", DEVICE_NAME, imajor(inodep), iminor(inodep));
    pr_info("%s - Device opened %d time(s)\n", DEVICE_NAME, open_count);

    return 0;
}

static ssize_t my_read(struct file *filep, char __user *user_buffer, size_t len, loff_t *offset)
{
    int not_copied, delta, to_copy = (len + *offset) < sizeof(message) ? len : (sizeof(message) - *offset);
    pr_info("%s - Read is called, we want to read %ld bytes, but actually only copying %d bytes. The offset is %lld\n", DEVICE_NAME, len, to_copy, *offset);
    
    if (*offset >= sizeof(message))
        return 0;

    not_copied = copy_to_user(user_buffer, message, message_len);
    delta = to_copy - not_copied;

    if (not_copied) {
        pr_info("%s - Could only copy %d bytes\n", DEVICE_NAME, delta);
    }

    *offset += delta;

    return delta;
}

static ssize_t my_write(struct file *filep, const char __user *user_buffer, size_t len, loff_t *offset)
{
    if (len > sizeof(message) - 1)
        len = sizeof(message) - 1;

    if (copy_from_user(message, user_buffer, len)) {
        pr_err("%s - Failed to receive message from user\n", DEVICE_NAME);

        return -EFAULT;
    }

    message[len] = '\0';
    message_len = len;

    pr_info("%s - Received message: %s\n", DEVICE_NAME, message);
    pr_info("%s - Sending data to Arduino...\n", DEVICE_NAME);

    return len;
}

static int my_release(struct inode *inodep, struct file *filep)
{
    pr_info("%s - Device closed\n", DEVICE_NAME);

    return 0;
}

static struct file_operations fops = {
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_release,
};

static int __init my_init(void)
{
    pr_info("%s - Initializing...\n", DEVICE_NAME);

    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        pr_err("%s - Error registering chrdev\n", DEVICE_NAME);
        return major;
    }

    pr_info("%s - Major Device Number: %d\n", DEVICE_NAME, major);

    arduino_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(arduino_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("%s - Error registering device class\n", DEVICE_NAME);
        return PTR_ERR(arduino_class);
    }

    arduino_device = device_create(arduino_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(arduino_device)) {
        class_destroy(arduino_class);
        unregister_chrdev(major, DEVICE_NAME);
        pr_err("%s - Error creating the device\n", DEVICE_NAME);
        return PTR_ERR(arduino_device);
    }

    pr_info("%s - Module loaded successfully\n", DEVICE_NAME);
    
    return 0;
}

static void __exit my_exit(void)
{
    device_destroy(arduino_class, MKDEV(major, 0));
    class_unregister(arduino_class);
    class_destroy(arduino_class);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("%s - Module unloaded successfully\n", DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);
