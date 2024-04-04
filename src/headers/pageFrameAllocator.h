/*
a map of all the usable memory in the system
for tracking which pages are in use and which are free
used to maintain in-use and free pages of the system.
*/
#pragma once

#include "bitmap.h"
#include "memoryMap.h"
#include "kheap.h"


class PageFrameAllocator
{
private:
    bool _is_initialized = false;
    uint64_t _used_memory = 0;
    uint64_t _free_memory = 0;
    uint64_t _reserved_memory = 0;

    BitMap _bitmap;

    // reserve page at address
    void reservePage(unsigned char *addr);

    // do not use this outside of pageFrameAllocator
    void unreservePage(unsigned char *addr);

    // free page at address
    void freePage(unsigned char *addr);

    // lock page at address
    void lockPage(const unsigned char* addr);

    unsigned char* requestPage();


public:
    // unsinged char* pointing to memory block

    bool init();

    unsigned long getFreeMem();
    unsigned long getUsedMem();
    unsigned long getReservedMem();

    // request a page from the allocator for use
    unsigned char* requestPages(unsigned int amount);

    // free number of pages
    void freePages(unsigned char *addr, unsigned int amount);

    // lock number of pages
    void lockPages(const unsigned char* addr, unsigned int amount);

    // do not use this outside of pageFrameAllocator
    void reservePages(unsigned char *addr, unsigned int amount);

    // do not use this outside of pageFrameAllocator
    void unreservePages(unsigned char *addr, unsigned int amount);

};


extern PageFrameAllocator phys_mem; // physical memory allocator
