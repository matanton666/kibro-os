#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "headers/virtualMemory.h"
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

		//* test printing to screen
		cls();
		print("Welcome to Kibro!\n");
		print(123456789);
		print('\n');
		print('A');
		print('\n');
		print((int)getCursur().x);
		print(',');
		print((int)getCursur().y);
	}
	else {
		write_serial((char*)"screen failed to initialize");
	}

	if (initPageFrameAllocator()) {
		write_serial((char*)"memory map initialized");

		//* test memory map
		print("\nfree memory: ");
		print((uint64_t)getFreeMem() / 1024);
		print("KB\n");
		print("reserved memory: ");
		print((uint64_t)getReservedMem() / 1024);
		print("KB\n");
		print("used memory: ");
		print((uint64_t)getUsedMem() / 1024);
		print("KB\n");

		// reserve the pages before the kernel just in case
		//reservePages(0, (uint64_t)KENREL_MEM_START / PAGE_SIZE - 1);
		write_serial("reserved pages");
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	initGdt();
	write_serial("initialized GDT");

	idt_init();
	write_serial("initialized IDT");

	//* test IDT
	//__asm("int $0x0e"); // pagefault

	initPaging();
}

