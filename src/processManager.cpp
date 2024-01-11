#include "processManager.h"
#include "headers/processManager.h"


PCB kernel_pcb;

PCB* current_task = nullptr;
PCB* last_task = nullptr;



void initMultitasking()
{
    asm volatile("cli");
    uint32_t stack_top;
    uint32_t cr3;

    // set up tss
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    asm volatile("mov %%esp, %0" : "=r"(stack_top));
    
    TSS kernel_tss = {
        .link = 0,
        .esp0 = (stack_top + (uint32_t)sizeof(TSS)),
        .ss0 = 0x10, // kernel data segment

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
    // set up pcb
    kernel_pcb.name = "kernel";
    kernel_pcb.state = RUNNING;
    kernel_pcb.tss = kernel_tss;
    kernel_pcb.high_priority = true;
    kernel_pcb.next_task = nullptr;

    // put the kernel tss in the gdt
    gdtSetGate(5, (uint32_t)(uintptr_t)&kernel_tss, sizeof(TSS), 0x89, 0x00);
    current_task = &kernel_pcb;
    // load tss to tr register
    asm volatile("ltr %%ax" : : "a"(0x28)); // tss descriptor offset in gdt
    asm volatile("sti");

}

void newTask(PCB* task, const char* name, uint32_t eip, uint32_t esp0, uint32_t esp3)
{
    memcpy(task, current_task, sizeof(PCB)); // copy base line for task
    // change the necessary fields
    task->tss.esp0 = esp0;
    task->tss.esp = esp3;
    task->tss.eip = eip;
    task->tss.eflags = 0x202;
    // set up task
    task->state = CREATED;
    task->name = name;
    // TODO: need atomic section here?
    task->next_task = current_task->next_task;
    current_task->next_task = task;
    task->state = RUNNABLE;
}

