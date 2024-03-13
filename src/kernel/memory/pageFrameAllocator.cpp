#include "../../headers/pageFrameAllocator.h"

PageFrameAllocator phys_mem;

unsigned char* PageFrameAllocator::requestPage()
{
    return requestPages(1);
}

unsigned char* PageFrameAllocator::requestPages(unsigned int amount)
{
    bool is_amount_free = true;

    if (amount == 0) {
        return nullptr;
    }

    // loop all indexes in bitmap until find free page
    for (unsigned long i = 0; i < _bitmap.getBufferSize() * 8; i++)
    {
        if (_bitmap.get(i) == PAGE_FREE) { // first page free
            is_amount_free = true;
            
            // check if `amount` amount of pages in a row are free
            for (int j = 1; j < amount && is_amount_free; j++)
            {
                if (_bitmap.get(i + j) != PAGE_FREE) 
                {
                    is_amount_free = false;
                }
            }
            
            if (is_amount_free)  // lock aount of pages if they are free
            {
                unsigned char* addr = (unsigned char*)(i * PAGE_SIZE);
                lockPages(addr, amount);
                return addr;
            }
        }
    }

    return nullptr; // todo: page frame swap when file system is available
}

bool PageFrameAllocator::init()
{
    if (_is_initialized) {
        return true;
    }

    if (!_bitmap.init()) { // bitmap fail to init
        return false;
    }

    _free_memory = _bitmap.getMemMapApi()->getFreeMem();
    write_serial_var("freemem", _free_memory);
    _used_memory = _bitmap.getMemMapApi()->getUsedMem();
    _reserved_memory = _bitmap.getMemMapApi()->getReservedMem();

    // lock bitmap pages
    lockPages(_bitmap.getBufferStartAddress(), _bitmap.getBufferSize() / PAGE_SIZE + 1);

    // reserve reserved memory
    MemoryMap* mem_map = _bitmap.getMemMapApi()->getMemoryMap();
    MemoryMapEntry* entry = mem_map->entries;

    while ((uint8_t*)entry < (uint8_t*)mem_map + mem_map->size) // loop all entries
    {
        if (entry->type != MULTIBOOT_MEMORY_AVAILABLE) {
            reservePages((unsigned char*)entry->base_addr, entry->length / PAGE_SIZE);
        }
        entry = (MemoryMapEntry*)((uint64_t)entry + mem_map->entry_size); // next entrie
    }
    
    _is_initialized = true;
    return true;
}

unsigned long PageFrameAllocator::getFreeMem()
{
    return _free_memory;
}

unsigned long PageFrameAllocator::getUsedMem()
{
    return _used_memory;
}

unsigned long PageFrameAllocator::getReservedMem()
{
    return _reserved_memory;
}

void PageFrameAllocator::freePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (_bitmap.get(index) == PAGE_LOCKED) {
        _bitmap.set(index, PAGE_FREE);
        _free_memory += PAGE_SIZE;
        _used_memory -= PAGE_SIZE;
    }
}

void PageFrameAllocator::lockPage(const unsigned char* addr)
{
    unsigned long index = (uint64_t)addr / PAGE_SIZE;
    if (_bitmap.get(index) == PAGE_FREE) {
        _bitmap.set(index, PAGE_LOCKED);
        _free_memory -= PAGE_SIZE;
        _used_memory += PAGE_SIZE;
    }
}

void PageFrameAllocator::freePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        freePage((unsigned char*)((uint64_t)addr + i));
    }
}

void PageFrameAllocator::lockPages(const unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        lockPage((unsigned char*)((uint64_t)addr + i));
    }
}

void PageFrameAllocator::unreservePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (_bitmap.get(index) == PAGE_LOCKED) {
        _bitmap.set(index, PAGE_FREE);
        _free_memory += PAGE_SIZE;
        _reserved_memory -= PAGE_SIZE;
    }
}

void PageFrameAllocator::reservePage(unsigned char *addr)
{
    unsigned long index = (unsigned long)addr / PAGE_SIZE;
    if (_bitmap.get(index) == PAGE_FREE) {
        _bitmap.set(index, PAGE_LOCKED);
        _free_memory -= PAGE_SIZE;
        _reserved_memory += PAGE_SIZE;
    }
}

void PageFrameAllocator::reservePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        reservePage((unsigned char*)((uint64_t)addr + i));
    }
}

void PageFrameAllocator::unreservePages(unsigned char *addr, unsigned int amount)
{
    for (unsigned int i = 0; i < amount*PAGE_SIZE; i += PAGE_SIZE)
    {
        unreservePage((unsigned char*)((uint64_t)addr + i));
    }
}


uintptr_t PageFrameAllocator::getBitmapEndAddress()
{
    return (uintptr_t)_bitmap.getBufferStartAddress() + _bitmap.getBufferSize() + 1;
}