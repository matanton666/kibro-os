#include <stdint.h>
#include "../headers/serial.h"
#include "../headers/screen.h"
#include "../headers/virtualMemory.h"
#include "../headers/std.h"
#include "../headers/memoryMap.h"
#include "../headers/pageFrameAllocator.h"
#include "../headers/GDT.h"
#include "../headers/IDT.h"
#include "../headers/processManager.h"


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
		print("\ncursur position: ");
		print((int)screen.getCursur().x);
		print(',');
		print((int)screen.getCursur().y);
		print('\n');
	}
	else {
		write_serial("**** screen failed to initialize");
	}

	initGdt();
	write_serial("init gdt");


	if (phys_mem.init()) {
		write_serial("memory map initialized");

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
		write_serial((char*)"**** memory map failed to initialize");
	}

	idt_init();
	write_serial("init idt");

	MemoryManager::PagingSystem kernelPaging;
	kernelPaging.initPaging(true);
	write_serial("init paging");

	process_manager.initMultitasking();
	write_serial("init multitasking");

	/*
	* prints and tests here:
	*/

	//* test context switch
	PCB* task1 = process_manager.newKernelTask((void*)testTask);
    print("task created, id: ");
    print(task1->id);
	print(", entry: 0x");
	printHex(task1->regs.eip);

    print("\nswitching to task\n");
    process_manager.contextSwitch();
    print("returned from task and testing switch again\n");
	
	write_serial("running task again");
    process_manager.contextSwitch();
    print("returned and finished switches\n");




	print("\n> ");
	while (true)
	{
	}
	write_serial("kernel finished!\n");
	
}
