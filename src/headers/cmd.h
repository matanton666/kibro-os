#pragma once

#include "std.h"
#include "virtualMemory.h"
#include "processManager.h"
#include "screen.h"
#include "shellCommands.h"
#include "moreCommands.h"


// todos:

// - create system to get string of command and run it accordingly
// - create demos for each of the parts of the os
// - create something to manage file system manuverablility
// - create text editor???
// - create buffer to hold screen contents and manage it (with queue?)

struct Command
{
    const char* name;
    void (*function)(char** args, unsigned int argCount);
    const char* description;
    const char* args; // space delemited argument names
};


void startShell();

// get input from user. reutrns false if input / failed
bool getInput(char* buffer, char* prompt, uint64_t bufferSize);

// converts input to char** of words. returns the number of words (does not allocate new memory)
unsigned int stringToScentence(char* input, char** output);

void cmd_help(char** args, unsigned int argCount);

void cmd_exit(char** args, unsigned int argCount);