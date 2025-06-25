# RoboticTEC - Custom Driver

The `custom_driver.c` file is the code for a custom driver that handles the communication with an Arduino. 

## How to compile and replace Arduino Driver
- Compile `arduino_custom_driver`
```
$ make
```

- Remove `cdc_acm` driver (this is the default Arduino driver - Make sure that's the right name) 

```
$ make cdc
```

- Load custom driver

```
$ make load
```

- Connect the Arduino to your PC
- Remove the default driver again

```
$ make cdc
```

- Verify that custom driver was successfully created and a device file was linked to the Arduino

```
$ make check
```

- If needed, go to `/static_library/bridge.c` and change the `CUSTOM_DEVICE` value.


## Additional

- If you want to unload the kernel module:

```
$ make unload
```
