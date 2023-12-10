#include "headers/IDT.h"
#include "IDT.h"

IdtPtr idtPtr;

void idtSetEntry(uint8_t vector, void *isr, uint8_t flags)
{
    IdtEntry* descriptor = (IdtEntry *)(idtPtr.base + vector * sizeof(IdtEntry));

    descriptor->isrLow = (uint32_t)isr & 0xFFFF;
    descriptor->kernelCS = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->flags = flags;
    descriptor->isrHigh = (uint32_t)isr >> 16;
    descriptor->zero = 0;
}

void idt_init()
{
    idtPtr.base = (uint32_t)requestPage();
    idtPtr.limit = (uint16_t)(sizeof(IdtEntry) * IDT_SIZE - 1);

    __asm__ volatile("cli");
    __asm__ volatile(
        "lidt %0" // Load the GDT using the lgdt instruction
        :
        : "m"(idtPtr) // Input: Operand for the lgdt instruction
    );
    // __asm__ volatile("lidt %0" : : "m"(idtPtr)); // load the new IDT
    // __asm__ volatile("sti");                   // set the interrupt flag
}