#include "../../headers/virtualMemory.h"

using MemoryManager::PagingSystem;

extern "C" void enable_paging(uintptr_t*);

MemoryManager::Page* PagingSystem::getPage(uintptr_t address, bool make, PageDirectory* dir)
{
    address /= PAGE_SIZE; // get index from address (get the left most 22 bits)
    uint32_t table_idx = address / PAGE_COUNT; // get table index from address (get the left most 10 bits)
    if (dir->pageTables[table_idx]) // if pageTable is there return page
    {
        return &dir->pageTables[table_idx]->pages[address % PAGE_COUNT]; // if page has been created return it (get the right most 10 bits)
    }
    else if (make)
    {
        //create pageTable and return page
        uintptr_t addr;
        dir->pageTables[table_idx] = (PageTable*)kmallocAlignedPhys(sizeof(PageTable), (uint32_t*)&addr);
        memset(dir->pageTables[table_idx], 0, PAGE_SIZE); // clean the memory
        dir->pageDirectoryEntries[table_idx] = addr | 0x3; // setting the permissions PRESENT, RW, SV.
        return &dir->pageTables[table_idx]->pages[address % PAGE_COUNT]; // return the page (get the right most 10 bits)
    }
    else
    {
        return nullptr;
    }
}

void PagingSystem::allocFrame(Page* page, bool user_supervisor, bool read_write)
{
    if (page->frame)
    {
        return; // if page already has a frame
    }
    else
    {
        uintptr_t addr = (uintptr_t)phys_mem.requestPages(1); // get free frame
        if (addr)
        {
            //set flags
            page->user_supervisor = user_supervisor ? 0 : 1;
            page->read_write = read_write ? 1 : 0;
            page->present = 1;
            page->frame = addr >> 12;// set frame
        }
        else
        {
            panic("No free frames!");
        }
    }
}

void PagingSystem::freeFrame(Page* page)
{
    uintptr_t frame;
    if (!(frame = page->frame))
    {
        return;
    }
    else
    {
        phys_mem.freePages((unsigned char*)(frame << 12), 1);
        page->frame = 0x0;
    }
}

void PagingSystem::identityPaging(PageDirectory* directory, uintptr_t start, uintptr_t end)
{
    phys_mem.reservePages((unsigned char*)start, ((unsigned int)(end - start)) / PAGE_SIZE);
    uintptr_t i = start;
    while (i < end)
    {
        Page* page = getPage(i, true, directory);
        page->user_supervisor = 0;
        page->read_write = 1;
        page->present = 1;
        page->frame = (i >> 12);// set frame
        i += PAGE_SIZE;
    }
}

void PagingSystem::initPaging(bool is_kernel)
{
    if (_is_initialized) return;

    // create page directory table and page table for the kernel
    PageDirectory* directory = (PageDirectory*)kmallocAligned(sizeof(PageDirectory));
    _currentDirectory = directory;
    memset(directory, 0, sizeof(PageDirectory));

    if(is_kernel)
    {
        //identity paging for kernel + 4MB
        identityPaging(directory, 0, (uintptr_t)(IDENTITY_PAGING_SIZE + KERNEL_MEM_END));
        //identity paging for framebuffer
        identityPaging(directory, screen.getFbStartAddress(), screen.getFbStartAddress() + screen.getFbLength());
    }
    
    enable_paging(directory->pageDirectoryEntries);
    _is_initialized = true;
}
