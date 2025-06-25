#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define CUSTOM_DEVICE "/dev/arduino_dev2"

static int driver_fd = -1;

int open_arduino_connection()
{
    if (driver_fd < 0) {
        driver_fd = open(CUSTOM_DEVICE, O_RDWR);
        if (driver_fd < 0) {
            perror("Failed to open device\n");

            return driver_fd;
        }
    }
    
    return 0;
}

int close_arduino_connection()
{
    if (driver_fd >= 0) {
        close(driver_fd);
        driver_fd = -1;
    }

    return 0;
}

int write_to_arduino(const char *message)
{
    if (driver_fd < 0) {
        int open = open_arduino_connection();

        if (open < 0)
            return open;
    }
    ssize_t write_result = write(driver_fd, message, strlen(message));
    if (write_result < 0) {
        perror("Faile to write to driver\n");

        return write_result;
    }

    printf("Wrote '%s' (%ld bytes) to %s\n", message, write_result, CUSTOM_DEVICE);

    int close = close_arduino_connection();

    return close;
}

int read_from_arduino(char *buffer, size_t size)
{
    if (driver_fd < 0) {
        int open = open_arduino_connection();

        if (open < 0)
            return open;
    }

    size_t n = read(driver_fd, buffer, size - 1);
    if (n >= 0) {
        buffer[n] = '\0';
        int close = close_arduino_connection();
        if (close < 0)
            return close;

        return (int)n;
    }

    int close = close_arduino_connection();

    return close;
}

int arduino_move_right() {
    return write_to_arduino("MR");
}

int arduino_move_left() {
    return write_to_arduino("ML");
}

int arduino_move_up() {
    return write_to_arduino("MU");
}

int arduino_move_down() {
    return write_to_arduino("MD");
}

int arduino_press_key() {
    return write_to_arduino("PK");
}
