#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "std.h"
#include "pageFrameAllocator.h"



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

	if (getMemoryMap()) {
		write_serial((char*)"memory map initialized");
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	print('\n');
	while ((uint8_t*)entries < (uint8_t*)memMap + memMap->size) // size of the memory map
    {
        print("base: ");
		print((uint64_t)entries->base_addr);
		print(" length: ");
		print((uint64_t)entries->length / 1024);
		print("KB type: ");
		entries->type == 1 ? print("available") : print("reserved");
		print('\n');
        entries = (MemoryMapEntry*)((uint64_t)entries + memMap->entry_size);
    }

	setMemorySizes();
	print("free memory: ");
	print((uint64_t)freeMemory / 1024);
	print("KB\n");
	print("reserved memory: ");
	print((uint64_t)reservedMemory / 1024);
	print("KB\n");
	
	
	while (true)
	{
		 
	}
}

