#include "../../headers/cmd.h"


Command commands[] = {
    {"help", cmd_help, "displays all commands", ""},
	{"exit", cmd_exit, "exits the shell", ""},
	// screen commands
	{"settext", cmd_settext, "set the text color", "color"},
	{"setbg", cmd_setbg, "set the background color", "color"},
	{"curwide", cmd_curwide, "toggle wide / slim cursor", ""},
	{"clear", cmd_clear, "clear the screen", ""},
	{"echo", cmd_echo, "repeat the given text", "text"},
	// pic commands
	{"sleep", cmd_sleep, "sleep x seconds", "seconds"},
	{"sleepms", cmd_sleepms, "sleep x miliseconds", "miliseconds"},
	// pci commands
	{"devs", cmd_devs, "prints the available hardware devices", ""},
	// memory commands
	{"pmem", cmd_pmem, "displays the physical memory usage", ""},
	{"vmem", cmd_vmem, "displays the virtual memory usage", ""},
	{"malloc", cmd_malloc, "allocates memory on heap", "size"},
	{"free", cmd_free, "frees memory from heap", "address"},
	{"paddr", cmd_paddr, "displays the physical address of a virtual address", "address"},
	{"px", cmd_px, "show the contents of an address (amount of addresses to show)", "address amount"},
	{"sx", cmd_sx, "set the contents of an address", "address value"},
	// process commands
	{"exec", cmd_exec, "executes a program in new process", "program_number"},
	{"programs", cmd_programs, "lists avilable programs that can be executed useing `exec`", ""},
	{"top", cmd_top, "displays the top processes", ""},
	{"kill", cmd_kill, "kills a process", "pid"},
};

/* disk commands

	{"ls", 0, "lists all files in the current directory"},
	{"cd", 0, "changes the current directory"},
	{"mkdir", 0, "creates a new directory"},
	{"rmdir", 0, "removes a directory"},
	{"rm", 0, "removes a file"},
	{"touch", 0, "creates a new file"},
	{"cat", 0, "displays the contents of a file"},
	{"write", 0, "writes to a file"},
	{"append", 0, "appends to a file"},
	{"mv", 0, "moves a file"},
	{"cp", 0, "copies a file"},
	{"rename", 0, "renames a file"},
	{"pwd", 0, "displays the current directory"},
	{"df", 0, "displays the amount of free space"},
	{"du", 0, "displays the amount of space used by a file"},
	{"find", 0, "finds a file"},
	{"grep", 0, "searches for a pattern in a file"},
	{"wc", 0, "displays the number of lines, words, and characters in a file"},	
*/

const int NUM_COMMANDS = sizeof(commands) / sizeof(commands[0]);


void startShell()
{
	char userInput[256];
	char* command;
	char** args;
	unsigned int argCount;

	screen.println("Welcome to Kibro!");

	while (true)
	{
		memset(userInput, 0, 256);
		if (!getInput(userInput, "> ", 256))
		{
			screen.println("*input too long*\n");
			continue;
		}

		argCount = stringToScentence(userInput, args);
		if (argCount != 0)
		{
			command = args[0];
			bool cmdFound = false;
			for (int i = 0; i < NUM_COMMANDS; i++)
			{
				if (strcmp(command, commands[i].name) == 0)
				{
					if (commands[i].function == 0)
					{
						screen.print("command not implemented: ");
						screen.println(command);
					}
					else
					{
						commands[i].function(args, argCount);
					}
					cmdFound = true;
					break;
				}
			}

			if (!cmdFound)
			{
				screen.print("command not found: ");
				screen.println(command);
			}
			
		}

		screen.newLine();
	}
	screen.cls();
	keyboard.reset();
}


bool getInput(char* buffer, char* prompt, uint64_t bufferSize)
{
	keyboard.reset();
	screen.print(prompt);
	uint64_t index = 0;
	while (true)
	{
		if (keyboard.hasBeenPressed()) 
		{
			char c = keyboard.getFromQueue();
			if (c == '\n') 
			{
				screen.print(c);
				buffer[index] = 0;
				return true;
			}
			else if (c == '\b') 
			{
				if (index > 0) 
				{
					buffer[index] = 0;
					index--;
					screen.print("\b");
				}
			}
			else 
			{
				screen.print(c);
				buffer[index] = c;
				index++;
				if (index >= bufferSize) 
				{
					buffer[index-1] = 0;
					return false;
				}
			}
		}
	}
	return false;
}


unsigned int stringToScentence(char* input, char** output)
{
	unsigned int wordCount = 0;
	char* wordStart = input;
	
	for (int i = 0; input[i] != 0; i++)
	{
		if (input[i] == ' ')
		{
			input[i] = 0;
			output[wordCount] = wordStart;
			wordCount++;
			wordStart = &input[i+1];
		}
	}
	if (wordStart != 0)
	{
		output[wordCount] = wordStart;
		wordCount++;
	}
	
	return wordCount;
}


void cmd_help(char** args, unsigned int argCount) 
{
	char** arguments = nullptr;

	screen.print("welcome to Kibro!, available commands:\n");
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        if (commands[i].function == 0)
        {
            screen.print("(not implemented) ");
        }
        screen.print(commands[i].name);
        screen.print(" - ");
        screen.print(commands[i].description);

		if (commands[i].args[0] != 0)
		{
			screen.print(" \\ args: ");
			int count = stringToScentence((char*)commands[i].args, arguments);
			for (int j = 0; j < count; j++)
			{
				screen.print("<");
				screen.print(arguments[j]);
				screen.print("> ");
			}
		}

		screen.newLine();
    }
}

void cmd_exit(char** args, unsigned int argCount) 
{
	screen.println("stopping all");
	write_serial("kernel finished!\n");
	cli();
	while (true)
	{
		asm("hlt");
	}
}