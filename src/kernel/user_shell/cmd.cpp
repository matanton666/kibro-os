#include "../../headers/cmd.h"



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
		if (command != 0) // TODO: problem with parsing 
		{
			// TODO: run command
			screen.print("commmmand: ");
			screen.print(command);

			screen.print("args: ");
			for (int i = 0; i < argCount; i++)
			{
				screen.print(args[i]);
				screen.print(" ");
			}

			screen.print("count: ");
			screen.print((int)argCount);
		}

		// int count = stringToScentence(userInput, args);
		// screen.print("args: ");
		// for (int i = 0; i < count; i++)
		// {
		// 	screen.print(args[i]);
		// 	screen.print(" ");
		// }

		screen.newLine();
	}
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
