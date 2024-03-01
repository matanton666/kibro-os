#include "../../headers/serial.h"



#define PORT 0x3f8          // COM1


 
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

void write_serial_uint(uint64_t num)
{
   char asString[40] = {0};
   uitoa(num, asString, 10);
   write_serial(asString);
}

void write_serial_hex(uint64_t num) {
   char asString[40] = {0};
   uitoa(num, asString, 16);
   write_serial(asString);
}

void write_serial_var(const char* name, uint64_t num) {
   for (int i = 0; name[i] != '\0'; i++)
	{
		write_serial_char(name[i]);
	}

   write_serial_char(':');
   write_serial_hex(num);
}

void write_serial_var(const char* name, char* str) {
   for (int i = 0; name[i] != '\0'; i++)
   {
      write_serial_char(name[i]);
   }

   write_serial_char(':');
   write_serial(str);
}