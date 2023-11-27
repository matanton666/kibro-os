#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "std.h"
#include "memoryMap.h"
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

	if (initMemoryMap()) {
		write_serial((char*)"memory map initialized");
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	print('\n');
	entrie = memMap->entries;
	while ((uint8_t*)entrie < (uint8_t*)memMap + memMap->size) // size of the memory map
    {
        print("base: ");
		print((uint64_t)entrie->base_addr);
		print(" length: ");
		print((uint64_t)entrie->length / 1024);
		print("KB type: ");
		entrie->type == 1 ? print("available") : print("reserved");
		print('\n');
        entrie = (MemoryMapEntry*)((uint64_t)entrie + memMap->entry_size); 
    }
	write_serial("printed memory map");

	print("free memory: ");
	print((uint64_t)getFreeMem() / 1024);
	print("KB\n");
	print("reserved memory: ");
	print((uint64_t)getReservedMem() / 1024);
	print("KB\n");
	print("used memory: ");
	print((uint64_t)getUsedMem() / 1024);
	print("KB\n");
	write_serial("printed memory state");

	
	while (true)
	{
		 
	}
}

