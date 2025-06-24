#include <stdio.h>
#include <stdbool.h>
#include "static_library/bridge.h"

int main(){
    int open_connection;
    int close_connection;
    int result;
    // First example: Write to arduino 4 times
    int counter = 0;
    bool isOn = true;

    while (counter < 4) {
        open_connection = open_arduino_connection();

        if (open_connection < 0) {
            return open_connection;
        }

        if (isOn) {
            result = write_to_arduino("LED_OFF");
        } else {
            result = write_to_arduino("LED_ON");
        }

        if (result < 0) {
            return result;
        }

        isOn = !isOn;

        counter++;

        close_connection = close_arduino_connection();

        if (close_connection < 0) {
            return close_connection;
        }

        sleep(2);
    }

    // Second example: Read from arduino
    open_connection = open_arduino_connection();

    if (open_connection < 0) {
        return open_connection;
    }

    char response[256];
    memset(response, 0, sizeof(response));

    result = read_from_arduino(response, sizeof(response));

    if (result < 0) {
        return result;
    }

    printf("Read from arduino: %s\n", response);

    close_connection = close_arduino_connection();

    if (close_connection < 0) {
        return close_connection;
    }

    // Third example: Move hand
    open_connection = open_arduino_connection();

    if (open_connection < 0) {
        return open_connection;
    }

    result = arduino_move_right();

    if (result < 0) {
        return result;
    }

    close_connection = close_arduino_connection();

    if (close_connection < 0) {
        return close_connection;
    }

    return 0;
}
