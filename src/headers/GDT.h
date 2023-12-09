#pragma once
#include "std.h"

struct gdtPtrStruct
{
    uint16_t size;               // The upper 16 bits of all selector limits.
    uint32_t base;                // The address of the first gdtEntry_t struct.
}
__attribute__((packed));
typedef struct gdtPtrStruct gdtPtr_t;

struct gdtEntryStruct
{
    uint16_t limit_low;           // The lower 16 bits of the limit.
    uint16_t base_low;            // The lower 16 bits of the base.
    uint8_t base_middle;         // The next 8 bits of the base.
    uint8_t access;              // Access flags, determine what ring this segment can be used in.
    uint8_t granularity;
    uint8_t base_high;           // The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdtEntryStruct gdtEntry_t;


void gdtSet(uint32_t gdtTable);
void gdtSetGate(unsigned short index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
void initGdt();