#pragma once
#include "std.h"
#include "serial.h"
#include "kheap.h"
#include "screen.h"

#define PAGE_COUNT 1024
#define TABLE_COUNT 1024
#define IDENTITY_PAGING_SIZE (4 * 1024 * 1024) // 4MB

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
} __attribute__((packed));

struct PageDirectory // page directory struct
{
	PageTable* pageTables[TABLE_COUNT]; // array of 1024 page tables

	uintptr_t pageDirectoryEntries[TABLE_COUNT]; // an array that contains the information about the page tables (present, read/write, etc.)
} __attribute__((packed)); //TODO: change type of pageDirectoryEntries to PageDirectoryEntry struct

extern PageDirectory* currentDirectory;

//returns the page using the virtual address and the page directory, creates the page if "make" is true
Page* getPage(uintptr_t address, bool make, PageDirectory* dir);
//allocates a frame to the provided page (using pageFrameAllocator)
void allocFrame(Page* page, bool user_supervisor, bool read_write);
//frees the frame of a page
void freeFrame(Page* page);
//identity paging
void identityPaging(PageDirectory* directory, uintptr_t start, uintptr_t end);
//initializes paging
void initPaging();

// TODO: make a namespace / api for the virtual memory functions
// TODO: test functions