#include "headers/serial.h"
#include "headers/std.h"


#define PORT 0x3f8          // COM1

void outb(unsigned short port, unsigned char value)
{
    __asm("outb %b0, %w1"
        :: "a"(value), "d"(port));
}
 
int init_serial() {
   outb(PORT + 1, 0x00);    // Disable all interrupts
   outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outb(PORT + 1, 0x00);    //                  (hi byte)
   outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
   outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
   outb(PORT + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   outb(PORT + 4, 0x0F);
   return 0;
}

 
void write_serial_char(char a) {
   outb(PORT,a);
}

void write_serial(const char* str) {
	for (int i = 0; str[i] != '\0'; i++)
	{
		write_serial_char(str[i]);
	}
   write_serial_char('\n');
}
void write_serial_int(int num)
{
   char asString[20] = {0};
   itoa(num, asString, 10);
   write_serial(asString);
}