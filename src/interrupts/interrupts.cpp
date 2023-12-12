#include "../headers/interrupts.h"
#include "interrupts.h"

__attribute__((interrupt)) void pagefaultHandler(struct InterruptFrame *frame, unsigned int errorCode)
{
    print("********pagefault exception detected********\n");
    print("error code: ");
    printBinary(errorCode);
    print('\n');

    __asm("cli; hlt");
}

__attribute__((interrupt)) void doubleFault(InterruptFrame *frame, unsigned int errorCode)
{
    print("********double fault detected********\n");
    print("error code: ");
    printBinary(errorCode);

    __asm("cli; hlt");
}
__attribute__((interrupt)) void generalFault(struct InterruptFrame *frame)
{
    print("********geleral fault detected********");

    __asm("cli; hlt");
}

__attribute__((interrupt)) void generalFaultWithErrCode(struct InterruptFrame *frame, unsigned int errorCode) {
    print("********geleral fault detected********\n");
    print("error code: ");
    printBinary(errorCode);

    __asm("cli; hlt");
}

