#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "std.h"
#include "memoryMap.h"
#include "pageFrameAllocator.h"
#include "GDT.h"
#include "IDT.h"
#include "processManager.h"


extern "C" void kernel_main(void) {
	getBootInfoAddressFromGrub();
	init_serial();
	write_serial((char*)"kernel booted");

	
	if (screen.init()) {
		write_serial((char*)"screen initialized");
	}
	else {
		write_serial((char*)"screen failed to initialize");
	}


	initGdt();
	write_serial("init gdt");
	initMultitasking();


	if (phys_mem.init()) {
		write_serial((char*)"memory map initialized");
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	// reserve the pages before the kernel just in case
	phys_mem.reservePages(0, (uint64_t)KENREL_MEM_START / PAGE_SIZE - 1);
	// lock the framebuffer and kernel pages
	phys_mem.lockPages((unsigned char*)screen.getFbStartAddress(), screen.getFbLength() / PAGE_SIZE);
    phys_mem.lockPages((unsigned char*)(uint64_t)KENREL_MEM_START, ((uint64_t)KERNEL_MEM_END - (uint64_t)KENREL_MEM_START) / PAGE_SIZE + 1);


	idt_init();
	write_serial("init idt");


	/*
	* prints and tests here:
	*/

	//* test printing to screen
	cls();
	print("Welcome to Kibro!\n");
	print(123456789);
	print('\n');
	print('A');
	print("\ncursur position: ");
	print((int)screen.getCursur().x);
	print(',');
	print((int)screen.getCursur().y);

	//* test memory map
	print("\nfree memory: ");
	print((uint64_t)phys_mem.getFreeMem() / 1024);
	print("KB\n");
	print("reserved memory: ");
	print((uint64_t)phys_mem.getReservedMem() / 1024);
	print("KB\n");
	print("used memory: ");
	print((uint64_t)phys_mem.getUsedMem() / 1024);
	print("KB\n\n");
	
	//* test page frame allocator
	print("kernel end address: ");
	print((uint64_t)KERNEL_MEM_END);
	print("\nrequesting pages with page frame allocator...\naddresses at start of requested pages ");
	print("(should start with 0):\n");
	for (int i = 0; i < 4; i++)
	{
		unsigned char* addr = phys_mem.requestPages(i);
		print((uint64_t)addr);
		print('\n');
	}
	
	//* test IDT
	// asm("int $0x0E"); // pagefault

	while (true)
	{
	}
	write_serial("kernel finished\n");
	
}

