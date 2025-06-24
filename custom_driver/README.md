# RoboticTEC - Custom Driver

The `custom_driver.c` file is the code for a custom driver that handles the communication with an Arduino. 

To compile and load the kernel module:

```
$ make
```

```
$ sudo insmod custom_driver.ko
```

To unload the kernel module:

```
$ sudo rmmod custom_driver
```
