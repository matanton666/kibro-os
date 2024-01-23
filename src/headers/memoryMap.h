/*
gets the memory map of all the memory in the system 
*/
#pragma once
#include "std.h"
#include "serial.h"

// types of memory regions
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

struct BasicMemoryInfo
{
    uint32_t type; // type = 4
    uint32_t size;
    uint32_t lower_mem; // starts at addr 0 
    uint32_t upper_mem; // starts at addr 1mb
}__attribute__((packed));

struct MemoryMapEntry
{
    uint64_t base_addr; // addr of the memory region (devided to low and high)
    uint64_t length; // length in bytes of the memory region (devided to low and high)
    uint32_t type; // type of memory region
    uint32_t reserved; // always 0
}__attribute__((packed));

struct MemoryMap
{
    uint32_t type; // type = 6
    uint32_t size;
    uint32_t entry_size; // size of each entry
    uint32_t entry_version; // should be 0
    struct MemoryMapEntry entries[0];
}__attribute__((packed));


class MemoryMapApi
{
private:
    MemoryMap* _mem_map = nullptr;
    BasicMemoryInfo* _mem_info = nullptr;
    MemoryMapEntry* _entrie = nullptr;

    uint64_t _used_memory = 0;
    uint64_t _free_memory = 0;
    uint64_t _reserved_memory = 0;
    uint64_t _largest_free_segment = 0;// pointer to largest memory segment

    bool _is_initialized = false;

    bool getMemoryMapFromBootloader();
    void getMemorySizes();

public:
    bool init();

    // get the memory map
    MemoryMap* getMemoryMap();

    // get the memory info
    BasicMemoryInfo* getMemoryInfo();

    uint64_t getUsedMem();
    uint64_t getFreeMem();
    uint64_t getReservedMem();
    uint64_t getLargestFreeSegment();
};


