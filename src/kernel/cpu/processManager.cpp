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

    cli();
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
    _kernel_pcb.state = RUNNABLE;
    _kernel_pcb.priority = HIGH_PRIORITY;
    _kernel_pcb.next = nullptr;
    _kernel_pcb.regs = {0};


    pushToQueue(&_kernel_pcb);
    _current_task = &_kernel_pcb;

    // put the kernel tss in the gdt
    gdtSetGate(5, (uint32_t)(uintptr_t)&_kernel_tss, sizeof(TSS), 0x89, 0x00);
    // load tss to tr register
    asm volatile("ltr %%ax" : : "a"(0x28)); // tss descriptor offset in gdt

    reloadGDT();
    _is_initialized = true;

    sti();
}

PCB* ProcessManagerApi::newTask(uint32_t entry, uint32_t* stack_ptr, uint32_t cr3, bool is_high_priority)
{
    // needed to be configured before context switch:
    // eip, cr3, esp
    cli();

    const int REGISTERS_ON_STACK = 7; // 7 registers are pushed on the stack when context switching 
    const int ARGUMENTS_ON_STACK = 2; // 2 arguments are pushed on the stack when context switching (og pcb and new pcb)

    // prepare task
    // PCB* task = (PCB*)phys_mem.requestPages(1);
    PCB* task = (PCB*)kmallocAligned(sizeof(PCB)); // TODO: virtual malloc this instead
    memset(task, 0, sizeof(PCB));
    task->state = CREATED;
    task->priority = is_high_priority;
    task->next = nullptr;
    task->id = getNewTaskID();

    task->regs.cr3 = cr3;
    task->regs.eip = (uint32_t)(uintptr_t)taskEntryFunction;
    task->func_ptr = (void(*)())entry;

    // prepare stack
    stack_ptr -= ARGUMENTS_ON_STACK; // move stack pointer back to make space for arguments
    *stack_ptr = task->regs.eip; // put eip on stack top so the return address of task switch is the begining of the new task
    *(stack_ptr - 1) = (uint32_t)(uintptr_t)task; // put task pointer on eax for entry function
    stack_ptr -= REGISTERS_ON_STACK; // move stack pointer back to make space for registers
    task->regs.esp = (uint32_t)(uintptr_t)stack_ptr; // set esp to the top of the stack

    // put task in queue
    task->state = CREATED;
    sti();

    return task;
}

PCB* ProcessManagerApi::newKernelTask(void* entry, bool is_high_priority)
{
    // TODO: find how much memory a stack needs for a task
    uint32_t cr3;
    // uint32_t* stack = (uint32_t*)phys_mem.requestPages(1);
    uint32_t* stack = (uint32_t*)kmallocAligned(PAGE_SIZE); // TODO: move this to virtual malloc
    stack = stack + PAGE_SIZE / sizeof(uint32_t) - 1; // move stack pointer to the top of the stack
    asm volatile("mov %%cr3, %0" : "=r"(cr3));

    return newTask((uint32_t)(uintptr_t)entry, stack, cr3, is_high_priority);
}

void ProcessManagerApi::contextSwitch(Queue<PCB>* lst)
{
    cli(); // sti occures at the end of switch_task function

    // figure which tasks to switch
    PCB* last_task = _current_task;
    PCB* upcoming_task = lst->pop();

    last_task->state = RUNNABLE;
    upcoming_task->state = RUNNING;

    _current_task = upcoming_task; // set for next context switch
    pushToQueue(upcoming_task); // set for next context switch

    write_serial("switching to task: ");
    write_serial_int(upcoming_task->id);
    write_serial(upcoming_task->priority ? " (high priority)\n" : " (low priority)\n");

    switch_task(last_task, upcoming_task);
}


void ProcessManagerApi::taskEntryFunction()
{
    PCB* task;
    uint32_t eax;

// pre execution 
    // get task pointer from EAX
    asm volatile("movl %%eax, %0" : "=r" (eax));
    task = (PCB*)eax;


// execute task
    task->func_ptr(); 


// post execution
    task->state = TERMINATED;
    process_manager.runNextTask();
}



unsigned int ProcessManagerApi::getNewTaskID()
{
    return ++_next_task_id;
}


void ProcessManagerApi::runNextTask()
{
    cli();

    // check if task that ran untill now is terminated
    if (_current_task->state == TERMINATED) {
        removeFromQueue(_current_task); // remove task that ran untill now

        // switch back to high priority just in case there is no more in low priority
        _before_last_priority = _last_priority;
        _last_priority = HIGH_PRIORITY;
        contextSwitch(&_high_priority_lst);
        return;
    }

    Queue<PCB>* curr_lst = &_high_priority_lst;
    
    if (curr_lst->peek() == nullptr) {
        return;
    }

    if (_low_priority_lst.peek() != nullptr && _last_priority == HIGH_PRIORITY && _before_last_priority == HIGH_PRIORITY) {
        // run low priority
        _before_last_priority = _last_priority;
        _last_priority = LOW_PRIORITY;
        curr_lst = &_low_priority_lst;
    }
    else { // run high priority
        _before_last_priority = _last_priority;
        _last_priority = HIGH_PRIORITY;
    }

    // check state of task
    switch (curr_lst->peek()->state)
    {
    case CREATED:
        // wait for it to be finished created
        curr_lst->rotate();
        runNextTask();
        break;
    
    case RUNNABLE:
        // run task
        contextSwitch(curr_lst);
        break;
    
    case BLOCKED:
        // wait
        curr_lst->rotate();
        runNextTask();
        break;
    
    case TERMINATED:
        // should not get here
        write_serial("something went wrong in process manager (process state is TERMINATED)");
        curr_lst->rotate();
        break;
    
    case RUNNING:
        // will get here if there is only one task in the list, or if there was a problem somewhere
        // this will sort itself out in context switch from low to high or from high to low
        break;

    default:
        break;
    }
        
    sti();
}


void ProcessManagerApi::pushToQueue(PCB* pcb)
{
    if (pcb->priority == HIGH_PRIORITY) { // high priority
        _high_priority_lst.push(pcb);
    }
    else { // low priority
        _low_priority_lst.push(pcb);
    }
}



void ProcessManagerApi::removeFromQueue(PCB* pcb)
{
    // TODO: free memory of pcb and task memory/stack
    if (pcb->priority == HIGH_PRIORITY) { // high priority
        _high_priority_lst.remove(pcb);
    }
    else { // low priority
        _low_priority_lst.remove(pcb);
    }
}


PCB* ProcessManagerApi::getCurrentTask()
{
    return _current_task;
}


void ProcessManagerApi::startTask(PCB* task)
{
    cli();
    task->state = RUNNABLE;
    pushToQueue(task);
    sti();

}




void testTask()
{
    PCB* task = process_manager.getCurrentTask();
    print("\ntask ");
    print(task->id);
    print(" sleeping for 2.5 seconds (");
    print(task->priority ? "high)\n" : "low)\n");

    pit.sleepS(2.5);

    print("\ntask ");
    print(task->id);
    print(" finished (");
    print(task->priority ? "high)\n" : "low)\n");
}