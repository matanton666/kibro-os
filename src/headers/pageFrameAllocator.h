/*
a map of all the usable memory in the system
for tracking which pages are in use and which are free
used to maintain in-use and free pages of the system.
*/
#pragma once

#include "std.h"
#include "memoryMap.h"
#include "kheap.h"

#define PAGE_SIZE 4096 // 4kib in bytes
#define PAGE_FREE false
#define PAGE_LOCKED true

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;
#define KERNEL_MEM_END (uintptr_t)(&_KernelEnd + PAGE_SIZE*3) // take 3 pages to make sure not overwriting anything
#define KENREL_MEM_START (uintptr_t)&_KernelStart

extern unsigned long bitmapBufferSize; // in bytes
extern unsigned char* bitmapBuffer;

extern bool isInitialized;

bool initPageFrameAllocator();

unsigned long getFreeMem();
unsigned long getUsedMem();
unsigned long getReservedMem();
// request a page from the allocator for use
unsigned char* requestPage();


// get value of index (if free or not)
bool bitmapGet(unsigned long index);
// set value of index (free / locked)
void bitmapSet(unsigned long index, bool value);



// unsinged char* pointing to memory block

// free page at address
void freePage(unsigned char *addr);
// lock page at address
void lockPage(unsigned char* addr);
// free number of pages
void freePages(unsigned char *addr, unsigned int amount);
// lock number of pages
void lockPages(unsigned char* addr, unsigned int amount);

// reserve page at address
void reservePage(unsigned char *addr);
// do not use this outside of pageFrameAllocator
void unreservePage(unsigned char *addr);
// do not use this outside of pageFrameAllocator
void reservePages(unsigned char *addr, unsigned int amount);
// do not use this outside of pageFrameAllocator
void unreservePages(unsigned char *addr, unsigned int amount);


