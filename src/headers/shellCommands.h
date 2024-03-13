#pragma once

#include "std.h"
#include "virtualMemory.h"
#include "processManager.h"
#include "screen.h"
#include "PCI.h"
#include "memoryAllocator.h"
#include "demoPrograms.h"


void cmd_settext(char** args, unsigned int argCount);

void cmd_setbg(char** args, unsigned int argCount);

void cmd_clear(char** args, unsigned int argCount);

void cmd_echo(char** args, unsigned int argCount);

void cmd_sleep(char** args, unsigned int argCount);

void cmd_sleepms(char** args, unsigned int argCount);

void cmd_devs(char** args, unsigned int argCount);

void cmd_pmem(char** args, unsigned int argCount);

void cmd_vmem(char** args, unsigned int argCount);

void cmd_malloc(char** args, unsigned int argCount);

void cmd_free(char** args, unsigned int argCount);

void cmd_paddr(char** args, unsigned int argCount);

void cmd_px(char** args, unsigned int argCount);

void cmd_sx(char** args, unsigned int argCount);

void cmd_programs(char** args, unsigned int argCount);

void cmd_exec(char** args, unsigned int argCount);

void cmd_top(char** args, unsigned int argCount);

void cmd_kill(char** args, unsigned int argCount);




// helper function for top
void printProcesses(PCB* head);


// TODO: file system

// TODO: demo programs







