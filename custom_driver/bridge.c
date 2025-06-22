#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CUSTOM_DEVICE "/dev/custom_driver"
#define ARDUINO_DEVICE "/dev/ttyACM0"
#define BAUDRATE B9600

int setup_serial(const char *device)
{
    int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening serial port");
        exit(EXIT_FAILURE);
    }

    struct termios tty;

    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr failed");
        close(fd);
        exit(EXIT_FAILURE);
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

int main()
{
    int arduino_fd = setup_serial(ARDUINO_DEVICE);

    printf("Bridge running. Arduino waiting for messages from Custom Kernel Module\n");

    char buffer[256];
    int counter = 0;

    while (1) {
        int driver_fd = open(CUSTOM_DEVICE, O_RDONLY);
        if (driver_fd < 0) {
            perror("Failed to open /dev/custom_driver\n");
            return EXIT_FAILURE;
        }
        ssize_t n = read(driver_fd, buffer, sizeof(buffer) - 1);

        if (n > 0) {
            printf("c = %c (%d)\n", buffer[0], buffer[0]);
            if (buffer[0] == 10) {
                printf("Working...\n");
            }
            buffer[n] = "\0";
            printf("DRIVER -> ARDUINO: %s\n", buffer);

            write(arduino_fd, buffer, n);
            write(arduino_fd, "\n", 1);
            counter++;
        } else if (n < 0) {
            perror("Error reading from driver");
            break;
        }

        if (counter > 4) {
            break;
        }
        close(driver_fd);
        sleep(2);
    }
    close(arduino_fd);

    return 0;
}
