#include "../../headers/shellCommands.h"


void cmd_settext(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("settext <text>");
        return;
    }
    screen.println(args[1]);
    // TODO:
}