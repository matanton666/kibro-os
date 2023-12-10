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
    idtPtr.base = (uint32_t)requestPage(); // TODO: make identity paging for this
    idtPtr.limit = (uint16_t)(sizeof(IdtEntry) * IDT_SIZE - 1);

    idtSetEntry(0xE, (void*)pagefaultHandler, IDT_TA_InterruptGate);

    __asm__ volatile(
        "lidt %0" // Load the IDT
        :
        : "m"(idtPtr) 
    );
    // __asm__ volatile("cli");
    // __asm__ __volatile__ ("sti");                   // set the interrupt flag
}