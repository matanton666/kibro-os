#include "../headers/interrupts.h"

__attribute__((interrupt)) void pagefaultHandler(struct interrupt_frame *frame)
{
    print("********pagefault exception detected********");
    __asm("cli; hlt");
}