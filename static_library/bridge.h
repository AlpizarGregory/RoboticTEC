#include <stdio.h>

int open_arduino_connection();

int close_arduino_connection();

int write_to_arduino(const char *message);

int read_from_arduino(char *buffer, size_t size);
