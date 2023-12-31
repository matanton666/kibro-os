#pragma once

#include "std.h"
#include "interrupts.h"

#define IDT_SIZE 256

#define IDT_INTERRUPT_GATE 0b10001110 // hardware interrupts (no interrupts while in handler)
#define IDT_TRAP_GATE 0b10001111 // software interrupts (interrupts enabled while in handler)
#define IDT_TASK_GATE 0b10000101 // hardware task switching


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