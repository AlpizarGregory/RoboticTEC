# RoboticTEC - Project 2

## Students 
- Gregory Alpizar
- Cristhofer Azofeifa

## Course
- CE4303 - Principios de Sistemas Operativos

# How to use it
- Connect an Arduino to your PC.
- Check the USB port being used by Arduino (usually `/dev/ttyACM0`).
- If using a different port, go to `/static_library/bridge.c` and change the `ARDUINO_DEVICE` value.
- Load the code to the Arduino (check `example.ino`).
- Go to `custom_driver` directory.
- Execute the following commands:

```
$ make
```

```
$ sudo insmod custom_driver.ko
```

- Go back to `RoboticTEC` directory.
- Execute the following commands:

```
$ make
```

```
$ make run
```

### Notes
- To ensure the right communication with the Arduino, the user must open the connection to Arduino, execute just method from `bridge.h`, then close the connection. The execution of multiples methods in the same connection could cause errors in the program.