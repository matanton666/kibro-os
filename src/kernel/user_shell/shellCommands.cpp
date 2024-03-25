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
    screen.print(uint32_t(phys_mem.getUsedMem() / MIB1));
    screen.print(" MIB\nfree mem: ");
    screen.print(uint32_t(phys_mem.getFreeMem() / MIB1));
    screen.print(" MIB\nreserved mem: ");
    screen.print(uint32_t(phys_mem.getReservedMem() / MIB1));
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

    screen.print("0x");
    screen.printHex(addrInt);
    screen.print(" = ");
    screen.print("0x");
    screen.printHex(valueInt);
}


void cmd_programs(char** args, unsigned int argCount)
{
    screen.println("programs:");
    for (int i = 0; i < NUM_PROGRAMS; i++)
    {
        screen.print(i);
        screen.print(") ");
        screen.print(programs[i].name);
        screen.print(" - ");
        screen.print(programs[i].description);
        screen.newLine();
    }
}


void cmd_exec(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("exec <program_number>");
        return;
    }

    char* program = args[1];
    int programInt = atoi(program);
    if (programInt < 0 || programInt >= NUM_PROGRAMS)
    {
        screen.println("invalid program number");
        return;
    }

    PCB* proc = process_manager.newKernelTask(programs[programInt].function, LOW_PRIORITY);
    screen.print("process id: ");
    screen.print(proc->id);
    screen.newLine();

    screen.println("running program...");
    process_manager.startTask(proc);
}


void cmd_top(char** args, unsigned int argCount)
{
    screen.println("running processes:");
    PCB* head = process_manager.getHighPriorityTask();
    printProcesses(head);

    head = process_manager.getLowPriorityTask();
    printProcesses(head);
}


void cmd_kill(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("kill <id>");
        return;
    }
    process_manager.killTask(atoi(args[1]));
}


void cmd_ls(char** args, unsigned int argCount)
{
    Directory* curr = getCurrentDir();
    if (curr == 0)
    {
        screen.println("failed to get current directory");
        return;
    }

    bool showAll = false;

    if (argCount > 1)
    {
        if (strcmp(args[1], "-a") == 0)
        {
            showAll = true;
        }
    }
    else // skip '.' and '..' directorys
    {
        curr = curr->next->next;
    }


    while (curr != nullptr)
    {
        screen.print(curr->entry.fileName);
        if (showAll)
        {
            screen.print(" ");
            switch (curr->entry.fileType)
            {
            case DirFileType::FT_DIR:
                screen.print("- DIR - ");
                break;
            case DirFileType::FT_RF:
                screen.print("- FILE - ");
                break;      
            case DirFileType::FT_SL:
                screen.print("- LINK - ");
                break;
            
            default:
                break;
            }

            int size = getInodeSize(getInode(curr->entry.InodeIdx));
            screen.print(size);
            screen.print("B");
        }
        screen.newLine();

        curr = curr->next;
    }
}


void cmd_cd(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("cd <dir>");
        return;
    }

    char* dir = args[1];
    if (!cd(dir))
    {
        screen.println("failed to change directory");
    }
}


void cmd_mkdir(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("mkdir <dir>");
        return;
    }

    char* dir = args[1];
    if (createNewDirectory(dir) != 0)
    {
        screen.println("failed to create directory");
    }
}


void cmd_rmdir(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("rmdir <dir>");
        return;
    }

    char* dir = args[1];
    write_serial(dir);
    if (!deleteDir(dir))
    {
        screen.println("failed to remove directory");
    }
}


void cmd_touch(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("touch <file>");
        return;
    }

    char* file = args[1];
    if (addDirEntryToPath(createDirEntry(file, DirFileType::FT_RF)) != 0)
    {
        screen.println("failed to create file");
    }
}


void cmd_rm(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("rm <file>");
        return;
    }

    char* file = args[1];
    if (!deleteFile(file))
    {
        screen.println("failed to remove file");
    }
}


void cmd_cat(char** args, unsigned int argCount)
{
    if (argCount < 2)
    {
        screen.println("cat <file>");
        return;
    }

    char* file = args[1];
    unsigned int fileSize = getFileSize(file);
    char* content = (char*)kernelPaging.getAllocator()->malloc(fileSize);

    if (readFromFile(file, (uint8_t*)content, fileSize) != 0)
    {
        screen.println("failed to read file");
        return;
    }

    screen.println(content);
}






void printProcesses(PCB* head)
{
    while (head != 0)
    {
        screen.print("id: ");
        screen.print(head->id);
        screen.print(", priority: ");
        screen.print(head->priority);
        screen.print(", state: ");
        screen.print(head->state);
        screen.print(", total mem: ");
        screen.print(head->paging_system->getAllocator()->getTotalMem() / KIB1);
        screen.print(" KIB");
        screen.newLine();
        head = head->next;
    }
}

