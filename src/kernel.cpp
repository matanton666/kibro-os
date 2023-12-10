#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "std.h"
#include "memoryMap.h"
#include "pageFrameAllocator.h"
#include "GDT.h"
#include "IDT.h"


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
	reservePages(0, (uint64_t)KENREL_MEM_START / PAGE_SIZE - 1);
	// lock the framebuffer and kernel pages
	lockPages((unsigned char*)fbInfo->addr, fbLength / PAGE_SIZE);
    lockPages((unsigned char*)(uint64_t)KENREL_MEM_START, ((uint64_t)KERNEL_MEM_END - (uint64_t)KENREL_MEM_START) / PAGE_SIZE + 1);

	initGdt();

	idt_init();

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


	print("\nfree memory: ");
	print((uint64_t)getFreeMem() / 1024);
	print("KB\n");
	print("reserved memory: ");
	print((uint64_t)getReservedMem() / 1024);
	print("KB\n");
	print("used memory: ");
	print((uint64_t)getUsedMem() / 1024);
	print("KB\n");
	
	print("requesting pages...\naddresses at start of requested pages:\n");
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

