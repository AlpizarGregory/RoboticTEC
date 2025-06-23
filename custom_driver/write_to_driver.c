#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#define CUSTOM_DEVICE "/dev/custom_driver"

int main()
{
    int counter = 0;
    bool isOn = true;
    ssize_t written;

    while (counter < 6) {
        int fd = open(CUSTOM_DEVICE, O_WRONLY);

        if (fd < 0) {
            perror("Failed to open device");
            return EXIT_FAILURE;
        }
        if (isOn) {
            written = write(fd, "LED_OFF\n", 8);
            isOn = false;
        } else {
            written = write(fd, "LED_ON\n", 7);
            isOn = true;
        }

        if (written < 0) {
            perror("Failed to write to device\n");
            close(fd);

            return EXIT_FAILURE;
        }

        printf("Wrote %ld bytes to %s\n", written, CUSTOM_DEVICE);
        counter++;
        close(fd);
        sleep(5);
    }

    return EXIT_SUCCESS;
}
