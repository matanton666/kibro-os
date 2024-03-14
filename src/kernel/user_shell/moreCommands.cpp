#include "../../headers/moreCommands.h"


void cmd_curwide(char** args, unsigned int argCount)
{
    if (screen.getCursorWidth() > 1)
    {
        screen.setCursorWidth(1);
    }
    else
    {
        screen.setCursorWidth(7);
    }
}