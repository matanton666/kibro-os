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

extern unsigned long bitmapBufferSize;
extern unsigned char* bitmapBuffer;

extern bool isInitialized;

bool initMemoryMap();

// get value of index
bool bitmapGet(unsigned long index);
// set value of index
void bitmapSet(unsigned long index, bool value);

// void* pointing to memory block

void freePage(void* addr);
void lockPage(void* addr);

void freePages(void* addr, unsigned int amount);
void lockPages(void* addr, unsigned int amount);

// do not use this outside of pageFrameAllocator
void reservePage(void* addr);
// do not use this outside of pageFrameAllocator
void unreservePage(void* addr);
// do not use this outside of pageFrameAllocator
void reservePages(void* addr, unsigned int amount);
// do not use this outside of pageFrameAllocator
void unreservePages(void* addr, unsigned int amount);
