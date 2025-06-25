#include <stdio.h>
#include <stdbool.h>
#include "static_library/bridge.h"

int main(){
    int result;
    // First example: Write to arduino 4 times
    int counter = 0;
    bool isOn = true;

    while (counter < 4) {
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

        sleep(5);
    }

    // Second example: Read from arduino
    char response[256];
    memset(response, 0, sizeof(response));

    result = read_from_arduino(response, sizeof(response));

    if (result < 0) {
        return result;
    }

    printf("Read from arduino: %s\n", response);

    // Third example: Move hand
    result = arduino_move_right();

    if (result < 0) {
        return result;
    }

    return 0;
}
