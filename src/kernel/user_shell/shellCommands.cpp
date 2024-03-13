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


void cmd_devs(char** args, unsigned int argCount)
{
    PCI pci;
    pci.printAvailableDevices();
}


void cmd_pmem(char** args, unsigned int argCount)
{
    screen.print("used mem: ");
    screen.print(phys_mem.getUsedMem() / MIB1);
    screen.print(" MIB\nfree mem: ");
    screen.print(phys_mem.getFreeMem() / MIB1);
    screen.print(" MIB\nreserved mem: ");
    screen.print(phys_mem.getReservedMem() / MIB1);
    screen.print(" MIB\n");
}


void cmd_vmem(char** args, unsigned int argCount)
{
    screen.print("used mem: ");
    screen.print(kernelPaging.getAllocator()->getUsedMem() / KIB1);
    screen.print(" KIB\nfree mem: ");
    screen.print(kernelPaging.getAllocator()->getFreeMem() / KIB1);
    screen.print(" KIB\nreserved mem: ");
    screen.print(kernelPaging.getAllocator()->getReservedMem() / KIB1);
    screen.print(" KIB\n");
}


void cmd_malloc(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("malloc <size>");
        return;
    }

    char* size = args[1];
    unsigned int sizeInt = atoi(size);
    uintptr_t addr = (uintptr_t)kernelPaging.getAllocator()->calloc(sizeInt);
    if (addr == 0)
    {
        screen.println("failed to allocate memory");
        return;
    }
    screen.print("allocated memory at: 0x");
    screen.printHex(addr);

    screen.newLine();
    uintptr_t addrInt = addr-8;
    unsigned int countInt = 10;
}


void cmd_free(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("free <addr>");
        return;
    }

    char* addr = args[1];
    if (addr[0] != '0' || addr[1] != 'x')
    {
        screen.println("invalid address (must start with 0x)");
        return;
    }

    uintptr_t addrInt = (uintptr_t)atoh(&addr[2]);
    kernelPaging.getAllocator()->free((void*)addrInt);
    screen.print("freed memory at: 0x");
    screen.printHex(addrInt);
}


void cmd_paddr(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("paddr <vaddr>");
        return;
    }

    char* addr = args[1];
    if (addr[0] != '0' || addr[1] != 'x')
    {
        screen.println("invalid address (must start with 0x)");
        return;
    }

    uintptr_t addrInt = (uintptr_t)atoh(&addr[2]);
    uintptr_t paddr = kernelPaging.translateAddr(addrInt);
    screen.print("physical address: 0x");
    screen.printHex(paddr);
}


void cmd_px(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("px <vaddr> <amount>");
        return;
    }

    char* addr = args[1];

    if (addr[0] != '0' || addr[1] != 'x')
    {
        screen.println("invalid address (must start with 0x)");
        return;
    }

    uintptr_t addrInt = (uintptr_t)atoh(&addr[2]);
    unsigned int countInt = 10;
    if (argCount > 2)
    {
        char* count = args[2];
        countInt = atoi(count);
    }

    // print the next `count` addresses like in a hex dump
    for (int i = 0; i < countInt; i++)
    {
        screen.print("0x");
        screen.printHex(addrInt);
        screen.print(" ");

        for (int j = 0; j < 4; j++)
        {
            if (*(unsigned char*)addrInt < 0x10)
            {
                screen.print("0");
            }
            screen.printHex(*(unsigned char*)addrInt);
            screen.print(" ");
            addrInt++;
        }
        screen.newLine();
    }
}


void cmd_sx(char** args, unsigned int argCount)
{
    if (argCount < 3)
    {
        screen.println("sx <vaddr> <value>");
        return;
    }

    char* addr = args[1];
    char* value = args[2];
    if (addr[0] != '0' || addr[1] != 'x')
    {
        screen.println("invalid address (must start with 0x)");
        return;
    }

    uintptr_t addrInt = (uintptr_t)atoh(&addr[2]);
    unsigned int valueInt = atoi(value);

    if (value[0] == '0' && value[1] == 'x')
    {
        valueInt = atoh(&value[2]);
    }

    // set the value at the address + offset
    *(uint32_t*)addrInt = valueInt;
}


