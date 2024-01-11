#pragma once

#include "std.h"
#include "GDT.h"
#include "screen.h"

// process states
enum ProcessState
{
    CREATED, // has been created but not yet run
    RUNNING, // is currently running
    BLOCKED, // is waiting but cannot be run
    RUNNABLE, // is waiting and can be run
    TERMINATED, // has finished running
};

// task state segment
struct TSS 
{
    uint16_t link; // segment selector for previous task
    uint16_t res_link; // reserved 

    uint32_t esp0; // privlage level 0 stack
    uint16_t ss0;
    uint16_t res_ss0; // reserved 

    uint32_t esp1; // privlage level 1
    uint16_t ss1;
    uint16_t res_ss1;

    uint32_t esp2; // privlage level 2
    uint16_t ss2;
    uint16_t res_ss2;

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
    uint16_t es;
    uint16_t res_es;
    uint16_t cs;
    uint16_t res_cs;
    uint16_t ds;
    uint16_t res_ds;
    uint16_t fs;
    uint16_t res_fs;
    uint16_t gs;
    uint16_t res_gs;
    uint16_t ldtr;
    uint16_t res_ldtr;
    uint16_t res_iopb;
    uint16_t iopb; // i/o premittions bitmap
}__attribute__((packed));


// process contorl block
struct PCB 
{
    TSS tss;
    uint8_t state;
    const char* name;
    bool high_priority;
    // TODO: add other fields to help with process managment

    PCB* next_task;
};

// create kernel task and put it in the gdt
void initMultitasking();

// create new task and put it next in queue
// eip: code entry point
// esp0: kernel stack pointer
// esp3: user stack pointer
void newTask(PCB* task, const char* name, uint32_t eip, uint32_t esp0, uint32_t esp3);
// TODO: create more customizeable task creation function

// TODO: context switch funciton