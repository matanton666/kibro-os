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

	if (initMemoryMap()) {
		write_serial((char*)"memory map initialized");
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	// reserve the pages before the kernel just in case
	reservePages(0, (uint64_t)&_KernelStart / PAGE_SIZE - 1);
	// lock the framebuffer and kernel pages
	lockPages((unsigned char*)fbInfo->addr, fbLength / PAGE_SIZE);
    lockPages((unsigned char*)(uint64_t)&_KernelStart, ((uint64_t)KERNEL_MEM_END - (uint64_t)&_KernelStart) / PAGE_SIZE + 1);



	// prints and tests here:
	cls();
	print("Welcome to Kibro!\n");
	print(123456789);
	print('\n');
	print('A');
	print('\n');
	print((int)getCursur().x);
	print(',');
	print((int)getCursur().y);

	
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


	print("free memory: ");
	print((uint64_t)getFreeMem() / 1024);
	print("KB\n");
	print("reserved memory: ");
	print((uint64_t)getReservedMem() / 1024);
	print("KB\n");
	print("used memory: ");
	print((uint64_t)getUsedMem() / 1024);
	print("KB\n");
	
	print("requesting pages:\n");
	for (int i = 0; i < 10; i++)
	{
		unsigned char* addr = requestPage();
		print((uint64_t)addr);
		print('\n');
	}

	while (true)
	{
		 
	}
}

