#include "../../headers/demoPrograms.h"

Progrm programs[] = {
    {"printAndSleep", (void*)prog_printAndSleep, "prints a message every 3 seconds for 10 times"},
    {"doNothing", (void*)prog_doNothing, "does nothing"},
};

int NUM_PROGRAMS = sizeof(programs) / sizeof(Progrm);


void prog_printAndSleep()
{
    for (int i = 0; i < 10; i++)
    {
        pit.sleepS(3);
        screen.println("\n3 seconds passed");
    }
}

void prog_doNothing()
{
    while (true)
    {
        asm("hlt");
    }
}