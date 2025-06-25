#include <linux/module.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

#define VENDOR_ID   0x2341  // Arduino Uno VID
#define PRODUCT_ID  0x0043  // Uno PID
#define DEVICE_NAME "arduino_dev"
#define CLASS_NAME  "arduino_usb"
#define MAX_USB_INTERFACES 2

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gregory & Cristofer");
MODULE_DESCRIPTION("Custom USB device driver for Arduino");

extern struct usb_driver arduino_driver;

/*
Specifies the vendor and product IDs that
the driver will handle.
*/
static struct usb_device_id arduino_table[] = {
    { USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
    { }
};

// Export arduino_table to user space
MODULE_DEVICE_TABLE(usb, arduino_table);

// Device context
struct arduino_device {
    struct usb_device *udev;
    struct usb_interface *interface;
    unsigned char *bulk_in_buffer;
    unsigned char *bulk_out_buffer;
    size_t bulk_in_size;
    size_t bulk_out_size;
    __u8 bulk_in_endpointAddr;
    __u8 bulk_out_endpointAddr;
    struct mutex io_mutex;
    struct usb_class_driver class;
};

// Pointer for each arduino
static struct arduino_device *dev_data[MAX_USB_INTERFACES];

static int arduino_open(struct inode *inodep, struct file *filep) {
    /*
    Called when the device file is opened from user space
    */
    struct usb_interface *interface;
    struct arduino_device *dev;
    int minor;

    minor = iminor(inodep);

    interface = usb_find_interface(&arduino_driver, minor);
    if (!interface) {
        pr_err("%s - USB interface for minor %d not found\n", CLASS_NAME, minor);
        return -ENODEV;
    }

    dev = usb_get_intfdata(interface);
    if (!dev)
        return -ENODEV;

    filep->private_data = dev;
    return 0;
}

static ssize_t arduino_read(struct file *filep, char __user *user_buffer, size_t len, loff_t *offset) {
    /*
    Called when the device file is read from user space
    */
    struct arduino_device *dev = filep->private_data;
    int retval = 0;
    int actual_length;

    if (len > dev->bulk_in_size) len = dev->bulk_in_size;
    retval = usb_bulk_msg(dev->udev,
                          usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
                          dev->bulk_in_buffer,
                          len,
                          &actual_length, 5000);
    if (!retval) {
        if (copy_to_user(user_buffer, dev->bulk_in_buffer, actual_length))
            return -EFAULT;
        return actual_length;
    }
    return retval;
}

static ssize_t arduino_write(struct file *filep, const char __user *user_buffer, size_t len, loff_t *offset) {
    /*
    Called when writing to device file
    */
    struct arduino_device *dev = filep->private_data;
    int retval = 0;
    int wrote_cnt = min(len, dev->bulk_out_size);

    if (copy_from_user(dev->bulk_out_buffer, user_buffer, wrote_cnt)) return -EFAULT;

    retval = usb_bulk_msg(dev->udev,
                          usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
                          dev->bulk_out_buffer,
                          wrote_cnt,
                          &wrote_cnt, 5000);
    return (retval ? retval : wrote_cnt);
}

static int arduino_release(struct inode *inode, struct file *file) {
    /*
    Called after finishig each operation
    */
    struct arduino_device *dev = file->private_data;
    mutex_unlock(&dev->io_mutex);
    return 0;
}

static struct file_operations fops = {
    .open = arduino_open,
    .read = arduino_read,
    .write = arduino_write,
    .release = arduino_release,
};

static int arduino_probe(struct usb_interface *interface, const struct usb_device_id *id) {
    /*
    Called if the system detects an Arduino connected to the PC
    */
    struct usb_device *udev = usb_get_dev(interface_to_usbdev(interface));
    struct usb_host_interface *iface_desc = interface->cur_altsetting;
    struct arduino_device *dev;
    struct usb_endpoint_descriptor *endpoint;
    int i, retval = -ENOMEM;

    dev = kzalloc(sizeof(*dev), GFP_KERNEL);
    if (!dev) return -ENOMEM;

    dev->udev = udev; 
    dev->interface = interface;
    mutex_init(&dev->io_mutex);

    dev->bulk_in_endpointAddr = dev->bulk_out_endpointAddr = 0;

    for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
        endpoint = &iface_desc->endpoint[i].desc;
        if (!dev->bulk_in_endpointAddr &&
            usb_endpoint_is_bulk_in(endpoint)) {
            dev->bulk_in_size = usb_endpoint_maxp(endpoint);
            dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
            dev->bulk_in_buffer = kmalloc(dev->bulk_in_size, GFP_KERNEL);
        }
        if (!dev->bulk_out_endpointAddr &&
            usb_endpoint_is_bulk_out(endpoint)) {
            dev->bulk_out_size = usb_endpoint_maxp(endpoint);
            dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
            dev->bulk_out_buffer = kmalloc(dev->bulk_out_size, GFP_KERNEL);
        }
    }
    if (!(dev->bulk_in_buffer && dev->bulk_out_buffer)) goto error;

    dev->class.name = DEVICE_NAME "%d";
    dev->class.fops = &fops;

    retval = usb_register_dev(interface, &dev->class);
    if (retval) goto error;

    usb_set_intfdata(interface, dev);
    dev_data[interface->minor] = dev;
    dev_info(&interface->dev, "Arduino-usb device now attached\n");
    return 0;

error:
    if (dev) {
        kfree(dev->bulk_in_buffer);
        kfree(dev->bulk_out_buffer);
        kfree(dev);
    }
    usb_put_dev(udev);
    return retval;
}

static void arduino_disconnect(struct usb_interface *interface) {
    /*
    Called when Arduino is disconnected
    */
    struct arduino_device *dev = usb_get_intfdata(interface);
    usb_deregister_dev(interface, &dev->class);
    usb_set_intfdata(interface, NULL);
    kfree(dev->bulk_in_buffer);
    kfree(dev->bulk_out_buffer);
    usb_put_dev(dev->udev);
    kfree(dev);
    dev_info(&interface->dev, "Arduino-usb device now disconnected\n");
}

struct usb_driver arduino_driver = {
    .name = "arduino_usb_driver",
    .id_table = arduino_table,
    .probe = arduino_probe,
    .disconnect = arduino_disconnect,
};
module_usb_driver(arduino_driver);
