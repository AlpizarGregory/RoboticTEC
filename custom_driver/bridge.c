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

int write_to_driver(char message[256])
{
    ssize_t written;
    int fd = open(CUSTOM_DEVICE, O_WRONLY);

    if (fd < 0) {
        perror("Failed to open device");

        return EXIT_FAILURE;
    }

    written = write(fd, message, strlen(message));

    if (written < 0) {
        perror("Failed to write to device\n");
        close(fd);

        return EXIT_FAILURE;
    }

    printf("Wrote '%s' (%ld bytes) to %s\n", message, written, CUSTOM_DEVICE);
    close(fd);

    return EXIT_SUCCESS;
}

int activate_bridge()
{
    char buffer[256];
    int arduino_fd = setup_serial(ARDUINO_DEVICE);
    if(arduino_fd < 0) {
        perror("Failed creating bridge\n");
        return arduino_fd;
    }

    int driver_fd = open(CUSTOM_DEVICE, O_RDONLY);
    if (driver_fd < 0) {
        perror("Failed to open /dev/custom_driver\n");
        return driver_fd;
    }

    ssize_t n = read(driver_fd, buffer, sizeof(buffer) - 1);

    if (n > 0) {
        buffer[n] = "\0";
        printf("DRIVER -> ARDUINO: %s\n", buffer);

        write(arduino_fd, buffer, n);
        write(arduino_fd, "\n", 1);
    } else if (n < 0) {
        perror("Error reading from driver");
    }

    close(driver_fd);
    close(arduino_fd);

    return 0;
}

int main()
{
    int counter = 0;
    bool isOn = true;

    while (counter < 6) {
        int write_result;
        if (isOn) {
            write_result = write_to_driver("LED_OFF");
        } else {
            write_result = write_to_driver("LED_ON");
        }
        if (write_result < 0) {
            return write_result;
        }
        isOn = !isOn;

        int bridge_result = activate_bridge();
        if (bridge_result < 0) {
            return bridge_result;
        }

        counter++;
        sleep(10);
    }

    return 0;
}
