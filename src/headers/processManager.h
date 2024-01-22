#pragma once

#include "GDT.h"
#include "screen.h"
#include "pageFrameAllocator.h"
#include "processManagmentStructs.h"

class ProcessManagerAPI
{
private:
    PCB* _current_task_PCB = nullptr;
    PCB* _last_task_PCB = nullptr;
    PCB _kernel_pcb = {0};
    unsigned int _next_task_id = 1;

    bool _is_initialized = false;

    unsigned int getNewTaskID();

public:

    // create kernel task and put it in the gdt
    void initMultitasking();

    // create new task and put it next in queue
    // entry: pointer to the tasks execution start
    // stack_ptr: pointer to top of stack (mem block)
    PCB* newTask(uint32_t entry, uint32_t* stack_ptr, uint32_t cr3, bool is_high_priority);

    // entry function for newKernelTask
    PCB* newKernelTask(void* entry);

    void contextSwitch();
};


// test functions for testing multitasking
void testTask();

extern ProcessManagerAPI process_manager;
