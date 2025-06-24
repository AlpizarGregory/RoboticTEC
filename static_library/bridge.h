#include <stdio.h>

int open_arduino_connection();

int close_arduino_connection();

int write_to_arduino(const char *message);

int read_from_arduino(char *buffer, size_t size);

int arduino_move_right();

int arduino_move_left();

int arduino_move_up();

int arduino_move_down();

int arduino_press_key();
