
#include "pageFrameAllocator.h"

unsigned long bitmapBufferSize = 0;
unsigned char* bitmapBuffer = nullptr;

bool isInitialized = false;


void bitmapSet(unsigned long index, bool value)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;
    
    // set bit to value
    bitmapBuffer[byteIndex] &= ~bit;
    if (value) {
        bitmapBuffer[byteIndex] |= bit;
    }
}

bool bitmapGet(unsigned long index)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;

    if ((bitmapBuffer[byteIndex] & bit) > 0) {
        return PAGE_LOCKED;
    }
    else {
        return PAGE_FREE;
    }
}

unsigned char *requestPage()
{
    // loop all indexes in bitmap untill find free page
    for (unsigned long i = 0; i < bitmapBufferSize * 8; i++)
    {
        if (bitmapGet(i) == PAGE_FREE) {
            unsigned char* addr = (unsigned char*)(i * PAGE_SIZE);
            lockPage(addr);
            return addr;
        }
    }

    return nullptr; // todo: page frame swap when file system is available
}

bool initMemoryMap()
{
    if (isInitialized) {
        return true;
    }
    isInitialized = true;

    if (!getMemoryMapFromBootloader()) {
        return false;
    }
    getMemorySizes();

    // 4kib memory per page and one bit representing each page, plus one just in case
    bitmapBufferSize = (freeMemory / PAGE_SIZE / 8) + 1; // in bytes
    bitmapBuffer = (unsigned char*)largestFreeSegment;// begins at the start of the largest memroy segment

    // make sure bitmap is after kernel code and not overwriting it
    if ((uint64_t)KERNEL_MEM_END > (uint64_t)bitmapBuffer) {
        bitmapBuffer = (unsigned char*)(uint64_t)KERNEL_MEM_END;
    }

    for (unsigned long i = 0; i < bitmapBufferSize; i++) // zero out memory
    {
        bitmapBuffer[i] = 0;
    }

    // lock bitmap pages
    lockPages(bitmapBuffer, bitmapBufferSize / PAGE_SIZE + 1);

    // reserve reserved memory
    entrie = memMap->entries;
    while ((uint8_t*)entrie < (uint8_t*)memMap + memMap->size) // loop all entries
    {
        if (entrie->type != MULTIBOOT_MEMORY_AVAILABLE) {
            reservePages((unsigned char*)entrie->base_addr, entrie->length / PAGE_SIZE + 1);
        }
        entrie = (MemoryMapEntry*)((uint64_t)entrie + memMap->entry_size); // next entrie
    }

    return true;
}

unsigned long getFreeMem()
{
    return freeMemory;
}

unsigned long getUsedMem()
{
    return usedMemory;
}

unsigned long getReservedMem()
{
    return reservedMemory;
}

void freePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_LOCKED) {
        bitmapSet(index, PAGE_FREE);
        freeMemory += PAGE_SIZE;
        usedMemory -= PAGE_SIZE;
    }
}

void lockPage(unsigned char* addr)
{
    unsigned long index = (uint64_t)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_FREE) {
        bitmapSet(index, PAGE_LOCKED);
        freeMemory -= PAGE_SIZE;
        usedMemory += PAGE_SIZE;
    }
}

void freePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        freePage((unsigned char*)((uint64_t)addr + i));
    }
}

void lockPages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        lockPage((unsigned char*)((uint64_t)addr + i));
    }
}

void unreservePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_LOCKED) {
        bitmapSet(index, PAGE_FREE);
        freeMemory += PAGE_SIZE;
        reservedMemory -= PAGE_SIZE;
    }
}

void reservePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_FREE) {
        bitmapSet(index, PAGE_LOCKED);
        freeMemory -= PAGE_SIZE;
        reservedMemory += PAGE_SIZE;
    }
}

void reservePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        reservePage((unsigned char*)((uint64_t)addr + i));
    }
}

void unreservePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        unreservePage((unsigned char*)((uint64_t)addr + i));
    }
}