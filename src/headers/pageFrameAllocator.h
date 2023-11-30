/*
a map of all the usable memory in the system
for tracking which pages are in use and which are free
used to maintain in-use and free pages of the system.
*/
#pragma once

#include "std.h"
#include "memoryMap.h"

#define PAGE_SIZE 4096 // 4kib in bytes
#define PAGE_FREE false
#define PAGE_LOCKED true

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;
#define KERNEL_MEM_END (&_KernelEnd + PAGE_SIZE*2) // take 2 pages to make sure not overwriteing anything

extern unsigned long bitmapBufferSize; // in bytes
extern unsigned char* bitmapBuffer;

extern bool isInitialized;

bool initMemoryMap();

unsigned long getFreeMem();
unsigned long getUsedMem();
unsigned long getReservedMem();


// get value of index
bool bitmapGet(unsigned long index);
// set value of index
void bitmapSet(unsigned long index, bool value);


// void* pointing to memory block

void freePage(void* addr);
void lockPage(unsigned char* addr);

void freePages(void* addr, unsigned int amount);
void lockPages(unsigned char* addr, unsigned int amount);

// do not use this outside of pageFrameAllocator
void reservePage(void* addr);
// do not use this outside of pageFrameAllocator
void unreservePage(void* addr);
// do not use this outside of pageFrameAllocator
void reservePages(void* addr, unsigned int amount);
// do not use this outside of pageFrameAllocator
void unreservePages(void* addr, unsigned int amount);


