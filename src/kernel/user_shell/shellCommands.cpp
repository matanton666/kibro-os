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


void cmd_clear(char** args, unsigned int argCount)
{
    screen.cls();
}


void cmd_echo(char** args, unsigned int argCount)
{
    for (int i = 1; i < argCount; i++)
    {
        screen.print(args[i]);
        screen.print(" ");
    }
}


void cmd_sleep(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("sleep <time>");
        return;
    }

    char* time = args[1];
    unsigned int timeInt = atoi(time);
    pit.sleepS(timeInt);
}

void cmd_sleepms(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("sleepms <time>");
        return;
    }

    char* time = args[1];
    unsigned int timeInt = atoi(time);
    pit.sleepMS(timeInt);
}