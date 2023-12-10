#include "../headers/interrupts.h"

__attribute__((interrupt)) void pagefaultHandler(struct interrupt_frame *frame)
{
    print("********pagefault exception detected********");
    __asm("cli; hlt");
}

__attribute__((interrupt)) void generalFault(struct interrupt_frame *frame)
{
    print("********geleral fault detected********");
    __asm("cli; hlt");
}
