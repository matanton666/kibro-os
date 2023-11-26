#include "memoryMap.h"

MemoryMap* memMap = nullptr;
BasicMemoryInfo* memInfo = nullptr;
extern MemoryMapEntry* entries = nullptr;
extern uint64_t freeMemory = 0;
extern uint64_t usedMemory = 0;
uint64_t reservedMemory = 0;

bool getMemoryMap()
{
    memInfo = getBootInfo<BasicMemoryInfo>(4);
    memMap = getBootInfo<MemoryMap>(6);
    entries = memMap->entries;

    if (memInfo == nullptr || memMap == nullptr || entries == nullptr) {
        return false;
    }
    return true;
}

void setMemorySizes()
{
    entries = memMap->entries;
    while ((uint8_t*)entries < (uint8_t*)memMap + memMap->size) // size of the memory map
    {
        if (entries->type == MULTIBOOT_MEMORY_AVAILABLE) {
            freeMemory += entries->length;
        }
        else {
            reservedMemory += entries->length;
        }
        entries = (MemoryMapEntry*)((uint64_t)entries + memMap->entry_size);
    }
}
