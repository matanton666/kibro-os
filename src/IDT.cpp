#include "headers/IDT.h"
#include "IDT.h"

IdtPtr idtPtr; 
IdtEntry idtEntries[IDT_SIZE];

void idtSetEntry(uint8_t vector, void *isr, uint8_t flags)
{
    IdtEntry* descriptor = (IdtEntry *)(idtPtr.base + vector * sizeof(IdtEntry));

    descriptor->isrLow = (uintptr_t)isr & 0xFFFF;
    descriptor->kernelCS = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->flags = flags;
    descriptor->isrHigh = (uintptr_t)isr >> 16;
    descriptor->zero = 0;
}

void idt_init()
{
    idtPtr.base = (uintptr_t)&idtEntries[0];
    idtPtr.limit = (uint16_t)(sizeof(IdtEntry) * IDT_SIZE - 1);

    idtSetEntry(0x00, (void *)devideByZeroHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x01, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x02, (void *)generalFault, IDT_INTERRUPT_GATE);

    idtSetEntry(0x03, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x04, (void *)overflowHandler, IDT_INTERRUPT_GATE);

    idtSetEntry(0x05, (void *)boundRangeExceededHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x06, (void *)invalidOpcodeHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x07, (void *)deviceNotAvailableHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x08, (void *)doubleFaultHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x09, (void *)generalFault, IDT_INTERRUPT_GATE);

    idtSetEntry(0x0a, (void *)invalidTSS_Handler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x0b, (void *)segmentNotPresentHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x0c, (void *)stackSegmentFaultHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x0d, (void *)generalProtectionFaultHandler, IDT_INTERRUPT_GATE);
    idtSetEntry(0x0e, (void *)pagefaultHandler, IDT_INTERRUPT_GATE);

    idtSetEntry(0x10, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x11, (void *)generalFaultWithErrCode, IDT_INTERRUPT_GATE);
    idtSetEntry(0x12, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x13, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x14, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x15, (void *)generalFaultWithErrCode, IDT_INTERRUPT_GATE);

    idtSetEntry(0x1b, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x1c, (void *)generalFault, IDT_INTERRUPT_GATE);
    idtSetEntry(0x1d, (void *)generalFaultWithErrCode, IDT_INTERRUPT_GATE);
    idtSetEntry(0x1e, (void *)generalFaultWithErrCode, IDT_INTERRUPT_GATE);
    idtSetEntry(0x0E, (void*)pagefaultHandler, IDT_INTERRUPT_GATE);


    __asm("cli");
    __asm__ volatile(
        "lidt %0" // Load the IDT
        :
        : "m"(idtPtr) 
    );
    __asm__ volatile("sti");
}
