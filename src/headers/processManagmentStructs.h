/*
strucures used for process managment
*/
#pragma once

#include "std.h"
#include "virtualMemory.h"

// process states
enum ProcessState
{
    CREATED, // has been created but not yet run
    RUNNING, // is currently running
    BLOCKED, // is waiting but cannot be run
    RUNNABLE, // is waiting and can be run
    TERMINATED, // has finished running
};

enum PRIORITY {
    LOW_PRIORITY = false,
    HIGH_PRIORITY = true,
};


// task state segment
struct TSS 
{
    uint32_t link; // segment selector for previous task 

    uint32_t esp0; // privlage level 0 stack
    uint32_t ss0; // put kernel data segment GDT entry here (should be 3th entry which is 0x10)

    uint32_t esp1; // privlage level 1
    uint32_t ss1;

    uint32_t esp2; // privlage level 2
    uint32_t ss2;

    // registers
    uint32_t cr3; 
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    
    // segments
    uint32_t es;
    uint32_t cs;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldtr;
    uint32_t iopb; // i/o premittions bitmap (just put the size of the struct here)
    uint32_t shadow_sp;
}__attribute__((packed));


struct Registers // ! DO NOT CHANGE THIS STRUCT
{
    uint32_t eip;
    uint32_t esp;
    uintptr_t* cr3;
    uint32_t eflags;
}__attribute__((packed));

// process contorl block
struct PCB // regular registers are stored on the stack
{
    Registers regs; // ! DO NOT MOVE THIS FIELD (it is used in assembly code)
    void (*func_ptr)(); // pointer to the function to run
    uint8_t state;
    int id;
    bool priority;
    PagingSystem* paging_system;

    PCB* next;
}__attribute__((packed));
