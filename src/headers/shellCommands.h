#pragma once

#include "std.h"
#include "virtualMemory.h"
#include "processManager.h"
#include "screen.h"
#include "PCI.h"
#include "memoryAllocator.h"


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




// TODO: memory manager

// TODO: process manager

// TODO: file system

// TODO: demo programs







