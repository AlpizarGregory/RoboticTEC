#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define CUSTOM_DEVICE "/dev/custom_driver"
#define ARDUINO_DEVICE "/dev/ttyACM0"
#define BAUDRATE B9600

static int driver_fd = -1;
static int arduino_fd = -1;

static int setup_serial(const char *device)
{
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening serial port");
        return -1;
    }

    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");
        close(fd);
        return -1;
    }

    cfsetospeed(&tty, BAUDRATE);
    cfsetispeed(&tty, BAUDRATE);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                         // disable break processing
    tty.c_lflag = 0;                                // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                                // no remapping, no delays
    tty.c_cc[VMIN]  = 1;                            // read blocks until 1 byte available
    tty.c_cc[VTIME] = 5;                            // timeout in deciseconds

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);                // ignore modem controls
    tty.c_cflag &= ~(PARENB | PARODD);              // no parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr failed");
        close(fd);
        exit(EXIT_FAILURE);
    }

    return fd;
}

int open_arduino_connection()
{
    driver_fd = open(CUSTOM_DEVICE, O_RDWR);
    if (driver_fd < 0) {
        perror("Failed to open device\n");

        return driver_fd;
    }

    arduino_fd = setup_serial(ARDUINO_DEVICE);
    if (arduino_fd < 0) {
        perror("Failed setting serial up\n");

        return arduino_fd;
    }

    return 0;
}

int close_arduino_connection()
{
    if (driver_fd >= 0) {
        close(driver_fd);
        driver_fd = -1;
    }

    if (arduino_fd >= 0) {
        close(arduino_fd);
        arduino_fd = -1;
    }

    return 0;
}

int write_to_arduino(const char *message)
{
    if (driver_fd >= 0 && arduino_fd >= 0) {
        ssize_t write_result = write(driver_fd, message, strlen(message));
        if (write_result < 0) {
            perror("Faile to write to driver\n");

            return write_result;
        }

        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = read(driver_fd, buffer, sizeof(buffer) - 1);

        if (n > 0) {
            buffer[n] = "\0";
            write(arduino_fd, buffer, n);
            write(arduino_fd, "\n", 1);
        } else if (n < 0) {
            perror("Error reading from driver\n");
        }

        printf("Wrote '%s' (%ld bytes) to %s\n", message, write_result, CUSTOM_DEVICE);

    }

    return 0;
}

int read_from_arduino(char *buffer, size_t size)
{
    if (driver_fd < 0) {
        return -1;
    }

    size_t n = read(driver_fd, buffer, size - 1);
    if (n >= 0) {
        buffer[n] = "\0";
        return (int)n;
    }

    return -1;
}
