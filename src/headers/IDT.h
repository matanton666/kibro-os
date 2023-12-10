#pragma once

#include "std.h"
#include "screen.h"
#include "pageFrameAllocator.h"
#include "serial.h"

#define IDT_SIZE 256

struct IdtEntry {
    uint16_t isrLow; // low 16 bits of ISR address
    uint16_t kernelCS; // code segment selector in kernel
    uint8_t zero; // reserved space 
    uint8_t flags; // attrebutes of the entry
    uint16_t isrHigh; // high 16 bits of ISR
} __attribute__((packed));

struct IdtPtr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));


void idtSetEntry(uint8_t vector, void* isr, uint8_t flags);
void idt_init();