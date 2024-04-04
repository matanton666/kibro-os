#include "../../headers/memoryMap.h"

MemoryMapApi mem_map_api;


bool MemoryMapApi::getMemoryMapFromBootloader()
{
    _mem_info = getBootInfo<BasicMemoryInfo>(4);
    _mem_map = getBootInfo<MemoryMap>(6);
    _entrie = _mem_map->entries;

    if (_mem_info == nullptr || _mem_map == nullptr || _entrie == nullptr) {
        return false;
    }
    return true;
}

// sets the memory size variables (freeMemory, largestFreeSegment)
void MemoryMapApi::getMemorySizes()
{
    _entrie = _mem_map->entries;
    uint64_t largestSegment = 0;

    while ((uint8_t*)_entrie < (uint8_t*)_mem_map + _mem_map->size) // loop all entries
    {
        if (_entrie->type == MULTIBOOT_MEMORY_AVAILABLE) {
            _free_memory += _entrie->length;
            // get largest free segment
            if (_entrie->length > largestSegment) {
                largestSegment = _entrie->length;
                _largest_free_segment = _entrie->base_addr;
            }
        }
        _entrie = (MemoryMapEntry*)((uint64_t)_entrie + _mem_map->entry_size); // next entrie
    }
}

bool MemoryMapApi::init()
{
    if (_is_initialized) {
        return true;
    }

    if (!getMemoryMapFromBootloader()) {
        return false;
    }
    getMemorySizes();
    
    _is_initialized = true;
    return true;
}

MemoryMap* MemoryMapApi::getMemoryMap()
{
    return _mem_map;
}

BasicMemoryInfo* MemoryMapApi::getMemoryInfo()
{
    return _mem_info;
}

uint64_t MemoryMapApi::getUsedMem()
{
    return _used_memory;
}

uint64_t MemoryMapApi::getFreeMem()
{
    return _free_memory;
}

uint64_t MemoryMapApi::getReservedMem()
{
    return _reserved_memory;
}

uint64_t MemoryMapApi::getLargestFreeSegment()
{
    return _largest_free_segment;
}
