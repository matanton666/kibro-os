/*
manage serial output from the os. (for debuging purposes)
*/
#pragma once

// serial code is for debuging the application
// send strings through the serial port to the serial.log file

#include <stdint.h>
#include "std.h"


int init_serial();
void write_serial_char(char a);
void write_serial(const char* str);
void write_serial_int(int num);
void write_serial_uint(uint64_t num);
void write_serial_hex(uint64_t num);
void write_serial_var(const char* name, uint64_t num);
