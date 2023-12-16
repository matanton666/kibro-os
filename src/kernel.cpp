#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "std.h"



extern "C" void kernel_main(void) {
	init_serial();
	write_serial((char*)"kernel booted");

	if (initializeScreen()) {
		write_serial((char*)"screen initialized");
	}
	else {
		write_serial((char*)"screen failed to initialize");
	}

	cls();
	print("Welcome to Kibro!\n");
	print(123456789);
	print('\n');
	print('A');
	print('\n');
	print((int)getCursur().x);
	print(',');
	print((int)getCursur().y);

	write_serial((char*)"screen works");

	
	while (true)
	{
		 
	}
}

