#include "../../headers/processManager.h"

ProcessManagerApi process_manager;

extern "C" {
    void switch_task(PCB* curr_task, PCB* next_task);
}


void ProcessManagerApi::initMultitasking()
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
    
    memset(&_kernel_tss, 0, sizeof(TSS));
    _kernel_tss.ss0 = 0x10; // offset in GDT of kernel data segment
    _kernel_tss.cr3 = cr3;
    // segmets
    _kernel_tss.es = 0x10;
    _kernel_tss.cs = 0x8;
    _kernel_tss.ds = 0x10;
    _kernel_tss.fs = 0x10;
    _kernel_tss.gs = 0x10;

    _kernel_tss.iopb = sizeof(TSS);


    // set up kernel PCB
    _kernel_pcb.id = 0;
    _kernel_pcb.state = RUNNING;
    _kernel_pcb.high_priority = true;
    _kernel_pcb.next_task = nullptr;
    _kernel_pcb.regs = {0};

    _current_task_PCB = &_kernel_pcb;
    _last_task_PCB = &_kernel_pcb;

    // put the kernel tss in the gdt
    gdtSetGate(5, (uint32_t)(uintptr_t)&_kernel_tss, sizeof(TSS), 0x89, 0x00);
    // load tss to tr register
    asm volatile("ltr %%ax" : : "a"(0x28)); // tss descriptor offset in gdt
    asm volatile("sti");

    reloadGDT();
    _is_initialized = true;
}

PCB* ProcessManagerApi::newTask(uint32_t entry, uint32_t* stack_ptr, uint32_t cr3, bool is_high_priority)
{
    // needed to be configured before context switch:
    // eip, cr3, esp

    const int REGISTERS_ON_STACK = 7; // 7 registers are pushed on the stack when context switching 
    const int ARGUMENTS_ON_STACK = 2; // 2 arguments are pushed on the stack when context switching (og pcb and new pcb)

    // prepare task
    // PCB* task = (PCB*)phys_mem.requestPages(1);
    PCB* task = (PCB*)kmallocAligned(sizeof(PCB)); // TODO: virtual malloc this instead
    memset(task, 0, sizeof(PCB));
    task->state = CREATED;
    task->high_priority = is_high_priority;
    task->next_task = nullptr;
    task->id = getNewTaskID();

    task->regs.cr3 = cr3;
    task->regs.eip = entry;

    // prepare stack
    stack_ptr -= ARGUMENTS_ON_STACK; // move stack pointer back to make space for arguments
    *stack_ptr = entry; // put eip on stack top so the return address of task switch is the begining of the new task
    stack_ptr -= REGISTERS_ON_STACK; // move stack pointer back to make space for registers
    task->regs.esp = (uint32_t)(uintptr_t)stack_ptr; // set esp to the top of the stack

    // put task in queue
    _last_task_PCB->next_task = task;
    _last_task_PCB = task;

    task->state = RUNNABLE;

    return task;
}

PCB* ProcessManagerApi::newKernelTask(void* entry)
{
    // TODO: find how much memory a stack needs for a task
    uint32_t cr3;
    // uint32_t* stack = (uint32_t*)phys_mem.requestPages(1);
    uint32_t* stack = (uint32_t*)kmallocAligned(PAGE_SIZE); // TODO: move this to virtual malloc
    stack = stack + PAGE_SIZE / sizeof(uint32_t) - 1; // move stack pointer to the top of the stack
    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    return newTask((uint32_t)(uintptr_t)entry, stack, cr3, false);
}

void ProcessManagerApi::contextSwitch()
{
    asm volatile("cli"); // sti occures at the end of switch_task function

    _last_task_PCB->next_task = _current_task_PCB;
    _current_task_PCB = _current_task_PCB->next_task;
    _last_task_PCB = _last_task_PCB->next_task;

    write_serial("context switch to task: ");
    write_serial_int(_current_task_PCB->id);
    switch_task(_last_task_PCB, _current_task_PCB);
}


unsigned int ProcessManagerApi::getNewTaskID()
{
    return ++_next_task_id;
}



void testTask()
{
    print("\n***    test task running   ***\n");
    process_manager.contextSwitch();
    print("\n***    test task running again     ***\n");
    process_manager.contextSwitch();
}