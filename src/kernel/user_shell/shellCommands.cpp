#include "../../headers/shellCommands.h"


void cmd_settext(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("settext <color>");
        return;
    }

    char* color = args[1];
    if (strcmp(color, "white") == 0)
    {
        screen.setTextColor(COLORS::WHITE);
    }
    else if (strcmp(color, "red") == 0)
    {
        screen.setTextColor(COLORS::RED);
    }
    else if (strcmp(color, "green") == 0)
    {
        screen.setTextColor(COLORS::GREEN);
    }
    else if (strcmp(color, "blue") == 0)
    {
        screen.setTextColor(COLORS::BLUE);
    }
    else if (strcmp(color, "yellow") == 0)
    {
        screen.setTextColor(COLORS::YELLOW);
    }
    else if (strcmp(color, "cyan") == 0)
    {
        screen.setTextColor(COLORS::CYAN);
    }
    else if (strcmp(color, "black") == 0)
    {
        screen.setTextColor(COLORS::BLACK);
    }
    else
    {
        screen.println("invalid color\nvalid colors: white, red, green, blue, yellow, cyan, black");
        return;
    }
}


void cmd_setbg(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("setbg <color>");
        return;
    }

    char* color = args[1];
    if (strcmp(color, "white") == 0)
    {
        screen.setBgColor(COLORS::WHITE);
    }
    else if (strcmp(color, "red") == 0)
    {
        screen.setBgColor(COLORS::RED);
    }
    else if (strcmp(color, "green") == 0)
    {
        screen.setBgColor(COLORS::GREEN);
    }
    else if (strcmp(color, "blue") == 0)
    {
        screen.setBgColor(COLORS::BLUE);
    }
    else if (strcmp(color, "yellow") == 0)
    {
        screen.setBgColor(COLORS::YELLOW);
    }
    else if (strcmp(color, "cyan") == 0)
    {
        screen.setBgColor(COLORS::CYAN);
    }
    else if (strcmp(color, "black") == 0)
    {
        screen.setBgColor(COLORS::BLACK);
    }
    else
    {
        screen.println("invalid color\nvalid colors: white, red, green, blue, yellow, cyan, black");
    }
    screen.cls();
}

