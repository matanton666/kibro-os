#pragma once
#include "std.h"


struct BasicMemoryInfo
{
    uint32_t type; // type = 4
    uint32_t size;
    uint32_t lower_mem; // starts at addr 0 
    uint32_t upper_mem; // starts at addr 1mb
}__attribute__((packed));


struct MemoryMap
{
    uint32_t type; // type = 6
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    uint32_t entries;
}__attribute__((packed));

struct MemoryMapEntry
{
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
}__attribute__((packed));




