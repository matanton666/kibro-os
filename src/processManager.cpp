#include "processManager.h"
#include "headers/processManager.h"

ProcessManagerAPI process_manager;

extern "C" {
    void switchTask(PCB* curr_task, PCB* next_task);
}


void ProcessManagerAPI::initMultitasking()
{
    if (_is_initialized) {
        return;
    }
    asm volatile("cli");
    uint32_t stack_top;
    uint32_t cr3;

    // set up basic tss because it is mandatory
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %%esp, %0" : "=r"(stack_top));
    
    TSS kernel_tss = {
        .link = 0,
        .esp0 = (stack_top + (uint32_t)sizeof(TSS)),
        .ss0 = 0x10, // kernel data segment GDT entry

        .esp1 = 0,
        .ss1 = 0,
        .esp2 = 0,
        .ss2 = 0,

        .cr3 = cr3,
        .eip = 0,
        .eflags = 0,
        .eax = 0,
        .ecx = 0,
        .edx = 0,
        .ebx = 0,
        .esp = 0,
        .ebp = 0,
        .esi = 0,
        .edi = 0,

        .es = 0x10,
        .cs = 0x8,
        .ds = 0x10,
        .fs = 0x10,
        .gs = 0x10,
        .ldtr = 0,
        .iopb = sizeof(TSS),
    };
    // set up kernel PCB
    _kernel_pcb.id = 0;
    _kernel_pcb.state = RUNNING;
    _kernel_pcb.high_priority = true;
    _kernel_pcb.next_task = nullptr;
    _kernel_pcb.regs = {0};

    _current_task_PCB = &_kernel_pcb;
    _last_task_PCB = &_kernel_pcb;

    // put the kernel tss in the gdt
    gdtSetGate(5, (uint32_t)(uintptr_t)&kernel_tss, sizeof(TSS), 0x89, 0x00);
    // load tss to tr register
    asm volatile("ltr %%ax" : : "a"(0x28)); // tss descriptor offset in gdt
    asm volatile("sti");

    reloadGDT();
    _is_initialized = true;
}

PCB* ProcessManagerAPI::newTask(uint32_t entry, uint32_t* stack_ptr, uint32_t cr3, bool is_high_priority)
{
    // needed to be configured before context switch:
    // eip, cr3, esp

    const int REGISTERS_ON_STACK = 7; // 7 registers are pushed on the stack when context switching 
    const int ARGUMENTS_ON_STACK = 2; // 2 arguments are pushed on the stack when context switching (og pcb and new pcb)

    PCB* task = (PCB*)phys_mem.requestPages(1); // TODO: malloc this instead
    // TODO: memset to 0
    task->state = CREATED;
    task->high_priority = is_high_priority;
    task->next_task = nullptr;
    task->id = getNewTaskID();

    task->regs.cr3 = cr3; // TODO: take care of cr3 when paging is implemented
    task->regs.eip = entry;

    stack_ptr -= ARGUMENTS_ON_STACK; // move stack pointer back to make space for arguments
    *stack_ptr = entry; // put eip on stack top so the return address of task switch is the begining of the new task
    stack_ptr -= REGISTERS_ON_STACK; // move stack pointer back to make space for registers
    task->regs.esp = (uint32_t)(uintptr_t)stack_ptr; // set esp to the top of the stack

    _last_task_PCB->next_task = task;
    _last_task_PCB = task;

    task->state = RUNNABLE;

    return task;
}

PCB* ProcessManagerAPI::newKernelTask(void* entry)
{
    uint32_t* stack = (uint32_t*)phys_mem.requestPages(1);
    stack = stack + PAGE_SIZE / sizeof(uint32_t) - 1; // move stack pointer to the top of the stack
    return newTask((uint32_t)(uintptr_t)entry, stack, 0, false);
}

void ProcessManagerAPI::contextSwitch()
{
    asm volatile("cli"); // sti occures at the end of switchTask function

    _last_task_PCB->next_task = _current_task_PCB;
    _current_task_PCB = _current_task_PCB->next_task;
    _last_task_PCB = _last_task_PCB->next_task;

    write_serial("context switch to task: ");
    write_serial_int(_current_task_PCB->id);
    switchTask(_last_task_PCB, _current_task_PCB);
}


unsigned int ProcessManagerAPI::getNewTaskID()
{
    return _next_task_id++;
}



void testTask()
{
    print("\ntest task running\n");
    process_manager.contextSwitch();
    print("\ntest task running again\n");
    process_manager.contextSwitch();
}