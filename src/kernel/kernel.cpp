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
#include "../headers/memoryAllocator.h"
#include "../headers/PCI.h"

extern PagingSystem kernelPaging;

void runTests();

extern "C" void kernel_main(void) {
	getBootInfoAddressFromGrub();
	init_serial();
	write_serial((char*)"\n**kernel booted**");

	if (screen.init()) {
		write_serial((char*)"screen initialized");

		//* test printing to screen
		screen.cls();
		screen.println("Welcome to Kibro!");
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
	}
	else {
		write_serial((char*)"**** memory map failed to initialize");
	}
	
	kernelPaging.kernelInit();
	write_serial("init paging");

    write_serial_var("kernel alllloc", (uint32_t)(uintptr_t)kernelPaging.getAllocator()->malloc(100));

	process_manager.initMultitasking();
	write_serial("init multitasking");

	PCI pci;
	pci.checkAllBuses(); // initialize the disk
	write_serial("enumerated pci"); 

	// * test disk
	uint8_t _buffer[0x50];
    uint8_t _buffer2[0x50];
    uint8_t _buffer3[0x100];
    // memset(_buffer, 67, 0x50);
    // memset(_buffer2, 65, 0x50);
    // disk.write(0x50, 0x50, _buffer);
    // disk.write(0, 0x50, _buffer2);
    disk.read(0, 0x100, _buffer3);

    screen.print((char*)_buffer3);


    write_serial_var("alllloc222**********", (uint32_t)(uintptr_t)kernelPaging.getAllocator()->malloc(100));
	// if (disk.getType() != PortType::NONE) {
	// 	write_serial("disk initialized");
	// }
	// else {
	// 	write_serial("disk failed to initialize");
	// }

	// test disk
	// char* _buffer = (char*)kernelPaging.getAllocator()->mallocAligned(256, 256);
	// memset(port._buffer, 65, 100);
	// port.write(0, 4, port._buffer);
	// disk->read(0, 4, (uint8_t*)_buffer);

	// for (int j = 0; j < 256 / sizeof(uint8_t); j++)
	// {
	// 	screen.print((char)_buffer[j]);
	// }


	// runTests();
	

	screen.print("\n>");
	while (true)
	{

	}
	write_serial("kernel finished!\n");
}



void runTests()
{
	/*
	* prints and tests here:
	*/

	write_serial("running tests");

	screen.print("\ncursur position: ");
	screen.print((int)screen.getCursur().x);
	screen.print(',');
	screen.print((int)screen.getCursur().y);
	screen.newLine();

	// * test memory map
	screen.print("\nfree memory: ");
	screen.print((uint64_t)phys_mem.getFreeMem() / 1024);
	screen.println("KB");
	screen.print("reserved memory: ");
	screen.print((uint64_t)phys_mem.getReservedMem() / 1024);
	screen.println("KB");
	screen.print("used memory: ");
	screen.print((uint64_t)phys_mem.getUsedMem() / 1024);
	screen.println("KB\n");
	
	//* test context switch
	screen.println("creating 5 tasks with different prioritys");
	write_serial("creating 5 tasks with different prioritys");

	process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));
	// process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));
	// process_manager.startTask(process_manager.newKernelTask((void*)testTask, HIGH_PRIORITY));
	// process_manager.startTask(process_manager.newKernelTask((void*)testTask, HIGH_PRIORITY));
	// process_manager.startTask(process_manager.newKernelTask((void*)testTask, LOW_PRIORITY));


	write_serial("kernel sleeping for 2 seconds");

	screen.println("\nkernel sleeping for 2 seconds"); 


	pit.sleepS(2.5);

	screen.println("\nkernel finished sleeping");
	write_serial("kernel finished sleeping");


	Allocator allocator;
	int size = 4 * 1024; // 4KB
	char memPool [size];
	uintptr_t p1 = (uintptr_t)memPool;



	screen.print("\nAllocated a block on the host, pointer: ");
	screen.printHex((uint64_t)p1);
	screen.print(", size ");
	screen.print(size);
	screen.newLine();
	
	allocator.init((uintptr_t)p1, size);
	
	void* t1 = 0;
	void* t6 = 0;

	void* t2 = allocator.malloc(1024);
	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)t2);
	screen.newLine();


	void* t3 = 0;

	void* t4 = allocator.malloc(300);
	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)t4);
	screen.newLine();


	screen.print("Allocating 500 (smaller than 800) \n");
	t3 = allocator.malloc(500);

	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)t3);
	screen.newLine();

	screen.print("Allocating same size as first pointer \n");
	t1 = allocator.malloc(100);
	
	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)t1);
	screen.newLine();

	screen.println("freeing all...");
	allocator.free(t2), t2 = nullptr;
	allocator.free(t1), t1 = nullptr;
	allocator.free(t3), t3 = nullptr;
	allocator.free(t4), t4 = nullptr;

	t1 = allocator.malloc(3000);
	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)t1);
	screen.newLine();

	allocator.free(t1); // test double freeing
	allocator.free(t1);

	void* adr = allocator.mallocAligned(0x10, 0x100);
	screen.print("Malloc'd from free list: ");
	screen.printHex((uint64_t)adr);
	screen.newLine();


	//test page fault
	struct a{int i;};
	a* idk = (a*)0x1507DC0B;

	idk->i = 5;
	screen.print("idk->i: ");
	screen.print((uintptr_t)idk->i);





}