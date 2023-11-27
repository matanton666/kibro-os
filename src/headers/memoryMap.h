/*
map of all the memory in the system devided into parts 
*/
#pragma once
#include "std.h"
#include "serial.h"


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

// types of memory regions
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

struct MemoryMap
{
    uint32_t type; // type = 6
    uint32_t size;
    uint32_t entry_size; // size of each entry
    uint32_t entry_version; // should be 0
    struct MemoryMapEntry entries[0];
}__attribute__((packed));


extern MemoryMap* memMap;
extern BasicMemoryInfo* memInfo;
extern MemoryMapEntry* entrie;

extern uint64_t usedMemory;
extern uint64_t freeMemory;
extern uint64_t reservedMemory;
extern void* largestFreeSegment; // pointer to uninitialized memory

bool getMemoryMapFromBootloader();
void getMemorySizes();


