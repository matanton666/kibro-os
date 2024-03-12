#include "../../headers/cmd.h"


Command commands[] = {
    {"help", cmd_help, "displays all commands"},
	{"exit", cmd_exit, "exits the shell"},
    {"echo", 0, "repeats user input"},
	{"clear", 0, "clears the screen"},
	{"mem", 0, "displays the amount of memory used"},
	{"ps", 0, "displays the current processes"},
	{"kill", 0, "kills a process"},
	{"run", 0, "runs a program"},
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
};

const int NUM_COMMANDS = sizeof(commands) / sizeof(commands[0]);


void startShell()
{
	char userInput[256];
	char* command;
	char** args;
	unsigned int argCount;

	keyboard.reset();
	while (true)
	{
		memset(userInput, 0, 256);
		if (!getInput(userInput, "> ", 256))
		{
			screen.println("*input too long*\n");
			continue;
		}

		argCount = parseCommand(userInput, &command, args);
		if (command != 0)
		{
			bool cmdFound = false;
			for (int i = 0; i < NUM_COMMANDS; i++)
			{
				if (strcmp(command, commands[i].name) == 0)
				{
					commands[i].function(args, argCount);
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

unsigned int parseCommand(char* input, char** command, char** args)
{
	unsigned int argCount = stringToScentence(input, args);

	*command = args[0];
	args = &args[1];
	return argCount-1;
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
    for (int i = 0; i < NUM_COMMANDS; i++)
    {
        screen.print(commands[i].name);
        screen.print(" - ");
        screen.print(commands[i].description);

        if (commands[i].function == 0)
        {
            screen.print(" (not implemented yet...)");
        }
		screen.newLine();
    }
}

void cmd_exit(char** args, unsigned int argCount) 
{
	screen.println("exiting...");
	write_serial("kernel finished!\n");
	cli();
	while (true)
	{
		asm("hlt");
	}
}