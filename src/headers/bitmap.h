/*
bitmap class for tracking which pages are in use and which are free
in the physica memory allocator and manager
*/
#pragma once


#include "std.h"
#include "memoryMap.h"

#define PAGE_SIZE 4096 // 4kib in bytes
#define PAGE_FREE false
#define PAGE_LOCKED true

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

#define END_OF_KERNEL_CODE ((uintptr_t)&_KernelEnd + PAGE_SIZE*5) // take 5 pages to make sure not overwriteing bss section (kernel stack)
#define START_OF_KERNEL_CODE ((uintptr_t)&(_KernelStart))

class BitMap
{
private:
    unsigned long _bufferSize = 0; // in bytes
    unsigned char* _buffer = nullptr; // bitmap buffer start address
    bool _is_initialized = false;
    MemoryMapApi _mem_map_api;


public:
    bool init();

    // get value of index (if free or not)
    bool get(unsigned long index);

    // set value of index (free / locked)
    void set(unsigned long index, bool value);

    // get the size of the bitmap buffer in bytes
    const unsigned long getBufferSize();

    // get the bitmap buffer
    const unsigned char* getBufferStartAddress();

    // get memory map api
    MemoryMapApi* getMemMapApi();

};