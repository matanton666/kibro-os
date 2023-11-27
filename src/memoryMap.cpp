#include "memoryMap.h"

MemoryMap* memMap = nullptr;
BasicMemoryInfo* memInfo = nullptr;
MemoryMapEntry* entrie = nullptr;

uint64_t freeMemory = 0;
uint64_t usedMemory = 0;
uint64_t reservedMemory = 0;
void* largestFreeSegment = nullptr;


bool getMemoryMapFromBootloader()
{
    memInfo = getBootInfo<BasicMemoryInfo>(4);
    memMap = getBootInfo<MemoryMap>(6);
    entrie = memMap->entries;

    if (memInfo == nullptr || memMap == nullptr || entrie == nullptr) {
        return false;
    }
    return true;
}

void getMemorySizes()
{
    entrie = memMap->entries;
    uint64_t largestSegment = 0;

    while ((uint8_t*)entrie < (uint8_t*)memMap + memMap->size) // loop all entries
    {
        if (entrie->type == MULTIBOOT_MEMORY_AVAILABLE) {
            freeMemory += entrie->length;
            // get largest free segment
            if (entrie->length > largestSegment) {
                largestSegment = entrie->length;
                largestFreeSegment = (void*)entrie->base_addr;
            }
        }
        else {
            reservedMemory += entrie->length;
        }
        entrie = (MemoryMapEntry*)((uint64_t)entrie + memMap->entry_size); // next entrie
    }
}
