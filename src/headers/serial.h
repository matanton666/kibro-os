#pragma once

// serial code is for debuging the application
// send strings through the serial port to the serial.log file

#include <stdint.h>

void outb(unsigned short port, unsigned char value);

int init_serial();
void write_serial_char(char a);
void write_serial(const char* str);
void write_serial_int(int num);