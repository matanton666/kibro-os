#include <stdint.h>
#include "serial.h"
#include "screen.h"
#include "headers/virtualMemory.h"
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
	}
	else {
		write_serial((char*)"screen failed to initialize");
	}

	initGdt();
	write_serial("init gdt");


	if (phys_mem.init()) {
		write_serial((char*)"memory map initialized");

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
	}
	else {
		write_serial((char*)"memory map failed to initialize");
	}

	idt_init();
	write_serial("init idt");

	MemoryManager::PagingSystem kernelPaging;
	kernelPaging.initPaging(true);
	// TODO: test paging
	write_serial("init paging");

	process_manager.initMultitasking();

	/*
	* prints and tests here:
	*/

	
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


	//* test context switch
	PCB* task1 = process_manager.newKernelTask((void*)testTask);
    print("\ntask created, id: ");
    print(task1->id);
    print("\nswitching to task\n");
    process_manager.contextSwitch();
    print("returned from task and testing switch again\n");
	
	write_serial("running task again");
    process_manager.contextSwitch();
    print("returned and finished switches\n");




	print("> ");
	while (true)
	{
	}
	write_serial("kernel finished!\n");
	
}

