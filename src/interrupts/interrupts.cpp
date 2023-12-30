#include "../headers/interrupts.h"
#include "interrupts.h"

const unsigned int EXCEPTION_COUNT = 32;
const char *exceptions[] = {
    "Divide by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "unknown",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "unknown",
    "x87 floating-point exception",
    "Alignment check",
    "Machine check",
    "SIMD floating-point exception",
    "Virtualization exception",
    "Control protection exception",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "unknown",
    "Hypervisor exception",
    "VMM communication exception",
    "Security exception",
    "unknown",
    "Triple fault",
    "unknown",
};


void remapPIC()
{
    uint8_t p1, p2;

    p1 = inb(PIC1_DATA); // save masks
    ioWait();
    p2 = inb(PIC2_DATA);
    ioWait();

    // init master pic chip
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // (in cascade mode)
    ioWait();

    // init slave pic chip
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); // (in cascade mode)
    ioWait();

    // set offsets in interrupt table
    outb(PIC1_DATA, 0x20); // master offset
    ioWait();
    outb(PIC2_DATA, 0x28); // slave offset
    ioWait();

    // tell master pic that there is a slave pic
    outb(PIC1_DATA, 0x04);
    ioWait();
    // tell slave pic its cascade identity
    outb(PIC2_DATA, 0x02);
    ioWait();

    // set 8086 mode
    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    // restore saved masks
    outb(PIC1_DATA, p1);
    outb(PIC2_DATA, p2);
}


__attribute__((no_caller_saved_registers)) void printException(unsigned int exceptionNumber, unsigned int errorCode)
{
    if (exceptionNumber < EXCEPTION_COUNT)
    {
        panic(exceptions[exceptionNumber]);
        print('\n');
        if (errorCode != 0)
        {
            print("Error code: ");
            printBinary(errorCode);
            print('\n');
        }
    }
    else
    {
        print("Unknown exception\n");
    }
}

__attribute__((no_caller_saved_registers)) void printSelectorError(SelectorError *err)
{
    print("External: ");
    print(err->external);
    print('\n');
    print("Table: ");
    print(err->table);
    print('\n');
    print("Index: ");
    print(err->index);
    print('\n');
}

__attribute__((interrupt)) void generalFault(struct InterruptFrame *frame)
{
    printException(EXCEPTION_COUNT, 0); // unknown exception
    asm("cli; hlt");
}

__attribute__((interrupt)) void generalFaultWithErrCode(struct InterruptFrame *frame, unsigned int errorCode)
{
    printException(EXCEPTION_COUNT, errorCode); // unknown exception

    asm("cli; hlt");
}

__attribute__((interrupt)) void devideByZeroHandler(InterruptFrame *frame)
{
    printException(0x00, 0);

    asm("cli; hlt");
}

__attribute__((interrupt)) void overflowHandler(InterruptFrame *frame)
{
    printException(0x04, 0);

    asm("cli; hlt");
}

__attribute__((interrupt)) void boundRangeExceededHandler(InterruptFrame *frame)
{
    printException(0x05, 0);

    asm("cli; hlt");
}

__attribute__((interrupt)) void invalidOpcodeHandler(InterruptFrame *frame)
{
    printException(0x06, 0);

    asm("cli; hlt");
}

__attribute__((interrupt)) void deviceNotAvailableHandler(InterruptFrame *frame)
{
    printException(0x07, 0);

    asm("cli; hlt");
}

__attribute__((interrupt)) void doubleFaultHandler(InterruptFrame *frame)
{
    printException(0x08, 0);

    print("cannot recover from double fault... please restart os\n");

    asm("cli; hlt");
}

__attribute__((interrupt)) void invalidTSS_Handler(InterruptFrame *frame, unsigned int errorCode)
{
    printException(0x0a, errorCode);

    asm("cli; hlt");
}

__attribute__((interrupt)) void segmentNotPresentHandler(InterruptFrame *frame, unsigned int errorCode)
{
    printException(0x0b, errorCode);

    asm("cli; hlt");
}

__attribute__((interrupt)) void stackSegmentFaultHandler(InterruptFrame *frame, unsigned int errorCode)
{
    printException(0x0c, errorCode);

    asm("cli; hlt");
}

__attribute__((interrupt)) void generalProtectionFaultHandler(InterruptFrame *frame, unsigned int errorCode)
{
    printException(0x0d, errorCode);

    SelectorError* error = (SelectorError*)&errorCode;
    printSelectorError(error);

    asm("cli; hlt");
}

__attribute__((interrupt)) void pagefaultHandler(struct InterruptFrame *frame, unsigned int errorCode)
{
    printException(0x0e, errorCode);
    PageFalutError* error = (PageFalutError*)&errorCode;
    //? can use error for more info on page fault      
    
    asm("cli; hlt");
}




