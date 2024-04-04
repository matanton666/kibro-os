#pragma once

#include "std.h"
#include "virtualMemory.h"
#include "processManager.h"
#include "screen.h"
#include "PCI.h"
#include "memoryAllocator.h"


struct Progrm
{
    const char* name;
    void* function;
    const char* description;
};


void prog_printAndSleep();

void prog_doNothing();



extern int NUM_PROGRAMS;
extern Progrm programs[];
