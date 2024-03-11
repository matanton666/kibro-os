#include "../headers/interrupts.h"

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

void picEndMaster()
{
    outb(PIC1_COMMAND, PIC_EOI);
}

void picEndSlave()
{
    outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI);
}


__attribute__((no_caller_saved_registers)) void printException(unsigned int exceptionNumber, unsigned int errorCode)
{
    if (exceptionNumber < EXCEPTION_COUNT)
    {
        screen.panic(exceptions[exceptionNumber]);
        screen.newLine();
        if (errorCode != 0)
        {
            screen.print("Error code: ");
            screen.printBinary(errorCode);
            screen.newLine();
        }
    }
    else
    {
        screen.print("Unknown exception\n");
    }
}

__attribute__((no_caller_saved_registers)) void printSelectorError(SelectorError *err)
{
    screen.print("External: ");
    screen.print(err->external);
    screen.newLine();
    screen.print("Table: ");
    screen.print(err->table);
    screen.newLine();
    screen.print("Index: ");
    screen.print(err->index);
    screen.newLine();
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

    screen.print("cannot recover from double fault... please restart os\n");

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
    PageFaultError* error = (PageFaultError*)&errorCode;

    
    uintptr_t faultAddr; // The faulting address is stored in the CR2 register.
    asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
    
    PagingSystem* pagingSystem = getCurrentPagingSys();
    if (pagingSystem != nullptr && pagingSystem->pageFaultHandler(error, faultAddr))
    {
        screen.print("handled page fault successfully \n");
        write_serial("handled page fault successfully \n");
    }
}


__attribute__((interrupt)) void keyboardInputHandler(struct InterruptFrame *frame) 
{
    uint8_t scancode = inb(KEYBOARD_INPUT_PORT);
    picEndMaster();

    keyboard.sendToQueue(scancode);
}


__attribute__((interrupt)) void PIT_InputHandler(struct InterruptFrame *frame) 
{
    pit.tick(); // tick once
    picEndMaster();

    if (pit.getTimeSinceBoot() % PROCESS_TIME == 0) { // context switch every 40 ms
        process_manager.runNextTask();
    
    }

}


