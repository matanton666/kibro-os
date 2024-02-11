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
#include "../headers/PIT.h"


extern "C" void kernel_main(void) {
	getBootInfoAddressFromGrub();
	init_serial();
	write_serial((char*)"kernel booted");


	
	if (screen.init()) {
		write_serial((char*)"screen initialized");

		//* test printing to screen
		cls();
		print("Welcome to Kibro!\n");
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

	idt_init();
	write_serial("init idt");


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

	MemoryManager::PagingSystem kernelPaging;
	kernelPaging.initPaging(true);
	write_serial("init paging");

	process_manager.initMultitasking();
	write_serial("init multitasking");

	/*
	* prints and tests here:
	*/

	//* test context switch
	print("creating 5 tasks with different prioritys\n");
	process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));
	process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));
	process_manager.startTask(process_manager.newKernelTask((void*)testTask, HIGH_PRIORITY));
	process_manager.startTask(process_manager.newKernelTask((void*)testTask, HIGH_PRIORITY));
	process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));


	print("\nkernel sleeping for 2 seconds\n");

	pit.sleepS(2);

	print("\nkernel finished sleeping\n");
	while (true)
	{
	}
	write_serial("kernel finished!\n");
	
}
