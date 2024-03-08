/*
manages the virtual memory of the system, used to set up paging and identity paging
*/
#pragma once
#include "std.h"
#include "serial.h"
#include "screen.h"
#include "memoryAllocator.h"
#include "pageFrameAllocator.h"

#define PAGE_COUNT 1024
#define TABLE_COUNT 1024
#define KENREL_HEAP_SIZE (MIB1*16) // 16MB
#define TOTAL_KERNEL_END_ADDR (KENREL_HEAP_SIZE + phys_mem.getBitmapEndAddress()) // kernel last memory address (including heap) // TODO: if doing lower memory kernel then change to 1GB


struct Page // basic page struct
{
	uint32_t present : 1;  // present
	uint32_t read_write : 1; // read/write
	uint32_t user_supervisor : 1; // user/supervisor
	uint32_t write_through : 1; // write through
	uint32_t cache_disable : 1; // cache disabled
	uint32_t accessed : 1; // accessed
	uint32_t dirty : 1; // dirty
	uint32_t page_size : 1 = 0; // page size (0 - 4KiB, 1 - 4MiB)
	uint32_t global : 1; // global
	uint32_t available : 3; // available
	uintptr_t frame : 20; // base address
} __attribute__((packed));

struct PageTable // page table struct
{
	Page pages[PAGE_COUNT]; // array of 1024 pages
} __attribute__((aligned(0x1000)));

struct PageDirectory // page directory struct
{
	PageTable* pageTables[TABLE_COUNT]; // array of 1024 page tables

	uintptr_t pageDirectoryEntries[TABLE_COUNT]; // an array that contains the information about the page tables (present, read/write, etc.) the actual pageDirectory
} __attribute__((packed));

struct PageFaultError { // make sure its alligned correctly and has right size
	uint16_t present : 1;
	uint16_t write : 1;
	uint16_t user : 1;
	uint16_t reserved_write : 1;
	uint16_t instruction_fetch : 1;
	uint16_t protection_key : 1;
	uint16_t shadow_stack : 1;
	unsigned char reserved : 8;
	uint16_t sgx : 1;
	uint16_t reserved2 : 15;
} __attribute__((packed));

class PagingSystem 
{
private:
	PageDirectory* _currentDirectory; // current process pageDirectory struct
	Allocator _alloc;
	int _is_initialized = -1;


public:

	bool init();

	void kernelInit();

	// returns the page using the virtual address and the page directory, creates the page if "make" is true
	Page* getPage(uintptr_t address, bool make);

	// allocates a frame to the provided page (using pageFrameAllocator)
	void allocFrame(Page* page, bool user_supervisor, bool read_write);

	// frees the frame of a page
	void freeFrame(Page* page);

	void mapKernelMem();

	// map a virtual address to the same physical address
	void identityPaging(uintptr_t start, uintptr_t end);
	
	//initializes paging
	void enable();

	bool pageFaultHandler(PageFaultError* pageFault, uintptr_t faultAddr);

	// create a new page directory in the kernel heap and set the _currentDirectory to it
	void createPageDirectory();

	// returns the current page directory
	PageDirectory* getCurrentDirectory() { return _currentDirectory; }

	// allocate physicl mem for tthis range of virtual addresses (and map them)
	void allocAddresses(uintptr_t start, uintptr_t end, bool user_supervisor, bool read_write);
	
	// free physical mem for this range of virtual addresses (and unmap them)
	void freeAddresses(uintptr_t start, uintptr_t end);
	
	Allocator* getAllocator() { return &_alloc; }

	// get the address to put in the cr3 register
	uintptr_t* getPageDirectoryAddr() { return (uintptr_t*)_currentDirectory->pageDirectoryEntries; }

	// translate a virtual address to a physical address
	uintptr_t translateAddr(uintptr_t virtualAddr);

	// map a virtual address to a physical address, returns the new virtual address
	uintptr_t mapVirtToPhys(uintptr_t virtualAddr, uintptr_t physicalAddr);

};

PagingSystem* getCurrentPagingSys();

extern PagingSystem kernelPaging;