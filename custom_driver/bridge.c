#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define CUSTOM_DEVICE "/dev/arduino_dev2"

int write_to_driver(bool isOn)
{
    ssize_t written;
    int fd = open(CUSTOM_DEVICE, O_WRONLY);

    if (fd < 0) {
        perror("Failed to open device");

        return EXIT_FAILURE;
    }

    if (isOn) {
        written = write(fd, "LED_OFF", 7);
    } else {
        written = write(fd, "LED_ON", 6);
    }

    if (written < 0) {
        perror("Failed to write to device\n");
        close(fd);

        return EXIT_FAILURE;
    }

    printf("Wrote %ld bytes to %s\n", written, CUSTOM_DEVICE);
    close(fd);

    return EXIT_SUCCESS;
}

int main()
{
    int counter = 0;
    bool isOn = true;

    while (counter < 6) {
        int write_result = write_to_driver(isOn);
        if (write_result < 0) {
            return write_result;
        }
        isOn = !isOn;

        counter++;
        sleep(5);
    }

    return 0;
}
