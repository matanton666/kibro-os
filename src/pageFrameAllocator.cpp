
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

bool initMemoryMap()
{
    if (isInitialized) {
        return true;
    }
    isInitialized = true;

    getMemoryMapFromBootloader();

    unsigned long memMapEntriesAmount = (memMap->size - sizeof(MemoryMap)) / memMap->entry_size;
    getMemorySizes();
    // 4kib memory per page and one bit representing each page, plus one just in case
    bitmapBufferSize = (freeMemory / PAGE_SIZE / 8) + 1; // in bits

    bitmapBuffer = (unsigned char*)largestFreeSegment;

    for (unsigned long i = 0; i < bitmapBufferSize; i++)
    {
        bitmapBuffer[i] = 0;
    }
    // free, lock and reserve
}


void freePage(void* addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_LOCKED) {
        bitmapSet(index, PAGE_FREE);
        freeMemory += PAGE_SIZE;
        usedMemory -= PAGE_SIZE;
    }
}

void lockPage(void* addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_FREE) {
        bitmapSet(index, PAGE_LOCKED);
        freeMemory -= PAGE_SIZE;
        usedMemory += PAGE_SIZE;
    }
}

void freePages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (int i = 0; i < amount; i += PAGE_SIZE)
    {
        freePage((void*)(address + i));
    }
}

void lockPages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (int i = 0; i < amount; i += PAGE_SIZE)
    {
        lockPage((void*)(address + i));
    }
}

void unreservePage(void* addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_LOCKED) {
        bitmapSet(index, PAGE_FREE);
        freeMemory += PAGE_SIZE;
        reservedMemory -= PAGE_SIZE;
    }
}

void reservePage(void* addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (bitmapGet(index) == PAGE_FREE) {
        bitmapSet(index, PAGE_LOCKED);
        freeMemory -= PAGE_SIZE;
        reservedMemory += PAGE_SIZE;
    }
}

void reservePages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (int i = 0; i < amount; i += PAGE_SIZE)
    {
        reservePage((void*)(address + i));
    }
}

void unreservePages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (int i = 0; i < amount; i += PAGE_SIZE)
    {
        unreservePage((void*)(address + i));
    }
}
