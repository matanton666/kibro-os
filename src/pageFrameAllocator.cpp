
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

    write_serial_hex((uint64_t)bitmapBuffer);
    for (unsigned long i = 0; i < bitmapBufferSize; i++) // zero out memory
    {
        bitmapBuffer[i] = 0;
    }

    // lock bitmap pages (kernel memory and bitmap memory)
    // TODO: lock the framebuffer
    lockPages((unsigned char*)(uint64_t)&_KernelStart, ((uint64_t)KERNEL_MEM_END - (uint64_t)&_KernelStart) / PAGE_SIZE + 1);
    lockPages(bitmapBuffer, bitmapBufferSize / PAGE_SIZE + 1);

    // reserve reserved memory
    entrie = memMap->entries;
    while ((uint8_t*)entrie < (uint8_t*)memMap + memMap->size) // loop all entries
    {
        if (entrie->type != MULTIBOOT_MEMORY_AVAILABLE) {
            reservePages((void*)entrie->base_addr, entrie->length / PAGE_SIZE + 1);
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

void freePage(void* addr)
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

void freePages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        freePage((void*)(address + i));
    }
}

void lockPages(unsigned char *addr, unsigned int amount)
{
    // TODO: make others like this
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        lockPage((unsigned char*)((uint64_t)addr + i));
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
    for (unsigned int i = 0; i < amount; i += PAGE_SIZE)
    {
        reservePage((void*)(address + i));
    }
}

void unreservePages(void *addr, unsigned int amount)
{
    unsigned long address = (unsigned long)addr;
    for (unsigned int i = 0; i < amount; i += PAGE_SIZE)
    {
        unreservePage((void*)(address + i));
    }
}
