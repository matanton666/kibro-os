/*
manage process creation and context switching
*/
#pragma once

#include "GDT.h"
#include "screen.h"
#include "pageFrameAllocator.h"
#include "processManagmentStructs.h"
#include "PIT.h"
#include "queue.h"
#include "interrupts.h"

#define PROCESS_STACK_START align_up((uint32_t)(TOTAL_KERNEL_END_ADDR + PAGE_SIZE), KIB4) // aligned start of the processes stack
#define PROCESS_STACK_INIT_SIZE MIB1// initial size of a processes stack (1MIB)
#define PROCESS_HEAP_START (PROCESS_STACK_START + PROCESS_STACK_INIT_SIZE + MIB1) // keep 1MIB of space for stack to grow
#define PROCESS_HEAP_INIT_SIZE (MIB1*2) // initial size of a processes heap (2MIB)



class ProcessManagerApi
{
private:
    Queue<PCB> _high_priority_lst;
    Queue<PCB> _low_priority_lst;
    Queue<PCB> _to_delete; // task to delete

    PCB* _current_task = nullptr; // current task running

    bool _last_priority = HIGH_PRIORITY; // last priority used
    bool _before_last_priority = HIGH_PRIORITY; // priority before last priority used

    PCB _kernel_pcb;
    TSS _kernel_tss;

    unsigned int _next_task_id = 1;
    bool _is_initialized = false;


    unsigned int getNewTaskID();

    // function used for entering a task by the context switch function
    static void taskEntryFunction();

    // push a pcb to the end of the queue
    // make sure to use CLI before this function
    void pushToQueue(PCB* pcb);

    // remove pcb from end of list
    // make sure to use CLI before this function
    void removeFromQueue(PCB* pcb);

    // switches between the tasks in the pcb linked list
    // assumes _high_priority_list / _low_priority_list not null and _current_task not null
    void contextSwitch(Queue<PCB>* lst);

    // create new task and put it next in queue
    // entry: pointer to the tasks execution start
    // stack_ptr: pointer to top of stack (mem block)
    PCB* newTask(uint32_t entry, uint32_t* stack_ptr, MemoryManager::PagingSystem* paging_sys, bool is_high_priority);


public:

    // create kernel task and put it in the gdt
    void initMultitasking();

    // entry function for newKernelTask (dont forget to free the memroy after use)
    PCB* newKernelTask(void* entry, bool is_high_priority);

    // run the next task in the queue (context switch)
    void runNextTask();

    // start a task (put it in the queue)
    void startTask(PCB* task);

    PCB* getCurrentTask();
};



// test functions for testing multitasking
void testTask();

extern ProcessManagerApi process_manager;