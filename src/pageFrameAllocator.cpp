#include "headers/pageFrameAllocator.h"

bool initMemoryMap()
{
    BasicMemoryInfo* memInfo = getBootInfo<BasicMemoryInfo>(4);

    MemoryMap* mmap = getBootInfo<MemoryMap>(6);
    MemoryMapEntry* entry = mmap->entries;

    write_serial_uint(memInfo->lower_mem);
    write_serial_uint(memInfo->upper_mem);

    write_serial_char('\n');

    while ((uint8_t*)entry < (uint8_t*)mmap + mmap->size)
    {
        write_serial_uint(entry->base_addr);
        write_serial_uint(entry->length);
        write_serial_uint(entry->type);
        write_serial_char('\n');
        entry = (MemoryMapEntry*)((uint64_t)entry + mmap->entry_size);
    }
    return false;
}
