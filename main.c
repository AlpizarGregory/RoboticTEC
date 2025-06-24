#include <stdio.h>
#include <stdbool.h>
#include "static_library/bridge.h"

int main(){
    int counter = 0;
    bool isOn = true;

    while (counter < 6) {
        int open_connection = open_arduino_connection();

        if (open_connection < 0) {
            return open_connection;
        }

        int write_result;

        if (isOn) {
            write_result = write_to_arduino("LED_OFF");
        } else {
            write_result = write_to_arduino("LED_ON");
        }

        if (write_result < 0) {
            return write_result;
        }

        isOn = !isOn;

        counter++;

        int close_connection = close_arduino_connection();

        if (close_connection < 0) {
            return close_connection;
        }

        sleep(10);
    }

    return 0;
}
