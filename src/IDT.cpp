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
    // memset((void *)idtPtr.base, 0, idtPtr.limit); //TODO: memset here when implemented

    for (int i = 0; i < 40; i++)
    { 
        idtSetEntry(i, (void*)generalFault, IDT_TA_InterruptGate);
    }
    idtSetEntry(0x0E, (void*)pagefaultHandler, IDT_TA_InterruptGate);
    

    __asm__ volatile(
        "lidt %0" // Load the IDT
        :
        : "m"(idtPtr) 
    );
    // __asm__ __volatile__ ("sti"); // FIXME: for some reason os generates interupt if use this command
}