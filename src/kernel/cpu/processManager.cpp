#include "../../headers/processManager.h"

ProcessManagerApi process_manager;

extern "C" {
    void switch_task(PCB* curr_task, PCB* next_task);
    void enable_paging(uintptr_t*);
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
    _kernel_pcb.paging_system = &kernelPaging;
    _kernel_pcb.regs.cr3 = kernelPaging.getPageDirectoryAddr();


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

PCB* ProcessManagerApi::newTask(uint32_t entry, uint32_t* stack_ptr, PagingSystem* paging_sys, bool is_high_priority)
{
    // needed to be configured before context switch:
    // eip, cr3, esp
    cli();

    const int REGISTERS_ON_STACK = 7; // 7 registers are pushed on the stack when context switching 
    const int ARGUMENTS_ON_STACK = 2; // 2 arguments are pushed on the stack when context switching (og pcb and new pcb)

    // prepare task
    PCB* task = (PCB*)kernelPaging.getAllocator()->callocAligned(sizeof(PCB), 256);

    task->state = CREATED;
    task->priority = is_high_priority;
    task->next = nullptr;
    task->id = getNewTaskID();

    task->regs.cr3 = paging_sys->getPageDirectoryAddr(); // set cr3 to the new page directory
    task->regs.eip = (uint32_t)(uintptr_t)ProcessManagerApi::taskEntryFunction;
    task->func_ptr = (void(*)())entry;
    task->paging_system = paging_sys;

    // kernel does not access the correct physical addresses here, need to temporarily move to the processes page directory
    
    task->paging_system->enable();
    
    // prepare stack
    stack_ptr -= ARGUMENTS_ON_STACK; // move stack pointer back to make space for arguments
    *stack_ptr = task->regs.eip; // put eip on stack top so the return address of task switch is the begining of the new task
    *(stack_ptr - 1) = (uint32_t)(uintptr_t)task; // put task pointer on eax for entry function
    stack_ptr -= REGISTERS_ON_STACK; // move stack pointer back to make space for registers
    task->regs.esp = (uint32_t)(uintptr_t)stack_ptr; // set esp to the top of the stack

    // move back to kernel page directory
    kernelPaging.enable();

    task->state = CREATED;

    write_serial_var("creating new task", task->id);
    sti();

    return task;
}

PCB* ProcessManagerApi::newKernelTask(void* entry, bool is_high_priority)
{
    cli();
    uint32_t stack_start = PROCESS_STACK_START; // address of start of stack
    uintptr_t heap_start_addr = PROCESS_HEAP_START; // address of start of heap

    PagingSystem* pg_sys = (PagingSystem*)kernelPaging.getAllocator()->callocAligned(sizeof(PagingSystem), KIB4); 
    if (pg_sys == nullptr) {
        write_serial("failed to allocate paging system for new task");
        return nullptr;
    }

    pg_sys->init();
    pg_sys->allocAddresses(stack_start, stack_start + PROCESS_STACK_INIT_SIZE, false, true); // map stack to new process
    pg_sys->allocAddresses(heap_start_addr, heap_start_addr + PROCESS_HEAP_INIT_SIZE, false, true);// map a total of 2 MIB for heap (keep 1 MIB space for stack to grow)
    // the initialization of the heap will happen in the entry function of the process

    uint32_t* stack_top = (uint32_t*)(uintptr_t)(stack_start); // move stack pointer to the top of the stack
    stack_top = stack_top + (PROCESS_STACK_INIT_SIZE / sizeof(uint32_t) - 4); // move stack pointer to the top of the stack
    return newTask((uint32_t)(uintptr_t)entry, stack_top, pg_sys, is_high_priority);
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

    // char buffer[20] = "switching to task: ";
    // buffer[18] = upcoming_task->id + '0';
    // buffer[19] = '\0';
    // write_serial(buffer);
    // write_serial(upcoming_task->priority ? " (high priority)\n" : " (low priority)\n");

    switch_task(last_task, upcoming_task);
}


void ProcessManagerApi::taskEntryFunction()
{
    cli();
    PCB* task;
    uint32_t eax;

    // pre execution 

    // get task pointer from EAX
    // asm volatile("movl %%eax, %0" : "=r" (eax));
    // task = (PCB*)eax;
    task = (PCB*)process_manager.getCurrentTask();

    task->paging_system->getAllocator()->init(PROCESS_HEAP_START, PROCESS_HEAP_INIT_SIZE); // init heap
    sti();

// execute task

    pit.sleepMS(PROCESS_TIME); // wait for a context switch to happen (for some reason if do not do this then pagefault will happen)
    task->func_ptr(); 

// post execution

    cli();
    task->state = TERMINATED;
    sti();
    while(true) {asm("hlt");}// wait for context switch
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
        _to_delete.push(_current_task); // set task to delete

        // switch back to high priority just in case there is no more in low priority
        _before_last_priority = _last_priority;
        _last_priority = HIGH_PRIORITY;
        contextSwitch(&_high_priority_lst);
        return;
    }

    if (!_to_delete.isEmpty() && _current_task == &_kernel_pcb) {
        while (!_to_delete.isEmpty())
        {
            PCB* to_del = _to_delete.pop();
            if (to_del == nullptr) {
                continue;
            }
        
            write_serial_var("deleteing task", to_del->id);
            // free process memory
            to_del->paging_system->freeAddresses(PROCESS_STACK_START, PROCESS_STACK_START + PROCESS_STACK_INIT_SIZE);
            to_del->paging_system->freeAddresses(PROCESS_HEAP_START, PROCESS_HEAP_START + PROCESS_HEAP_INIT_SIZE);
            kernelPaging.getAllocator()->free(to_del->paging_system);
            kernelPaging.getAllocator()->free(to_del); 
            to_del = nullptr;
        }
    }

    Queue<PCB>* curr_lst = &_high_priority_lst; // next task
    
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
        _to_delete.push(curr_lst->pop());
        runNextTask();
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

void ProcessManagerApi::killTask(PCB* task)
{
    cli();
    task->state = TERMINATED;
    sti();
}

void ProcessManagerApi::killTask(unsigned int id)
{
    cli();
    PCB* task = _high_priority_lst.peek();
    while (task != 0 && task->id != id) task = task->next;
    
    if (task == nullptr) {
        task = _low_priority_lst.peek();
        while (task != 0 && task->id != id) task = task->next;
    }
    if (task != nullptr) {
        task->state = TERMINATED;
    }
    sti();
}


PCB* ProcessManagerApi::getHighPriorityTask()
{
    return _high_priority_lst.peek();
}

PCB* ProcessManagerApi::getLowPriorityTask()
{
    return _low_priority_lst.peek();
}



void testTask()
{
    // write_serial("test task running\n");
    // screen.print("test task running\n");
    PCB* task = process_manager.getCurrentTask();
    uint32_t rand = task->id;
    task->paging_system->getAllocator()->malloc(sizeof(int)*rand);
    int* i = (int*)task->paging_system->getAllocator()->malloc(sizeof(int));
    *i = 1234 * task->id;
    pit.sleepS(1);
    screen.print(*i);
    screen.print(";  ");
    screen.print((uint64_t)i);
    screen.print(";  ");
    screen.printHex(task->paging_system->translateAddr((uintptr_t)i));

    screen.newLine();
    task->paging_system->getAllocator()->free(i);

    screen.print("task: ");
    screen.print(task->id);
    screen.newLine();
}