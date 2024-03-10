#include "../../headers/virtualMemory.h"

PagingSystem* _currentPagingSys;
PagingSystem kernelPaging;

extern "C" void enable_paging(uintptr_t*);
extern "C" void switch_pd(uintptr_t*);


bool PagingSystem::init()
{
    if (_is_initialized == -1) {
        return true;
    }

    createPageDirectory();
    if (getPageDirectoryAddr() == nullptr) {
        return false;
    }

    mapKernelMem();

    _is_initialized = -1;
    return true;
} 


void PagingSystem::kernelInit() // TODO: find a better way to initialize the kernel and processes seprately
{
    if (_is_initialized) {
        return;
    }
    _alloc.init(phys_mem.getBitmapEndAddress(), KENREL_HEAP_SIZE); // init allocator for kernel heap

    // createPageDirectory();
    write_serial_var("alloc start", phys_mem.getBitmapEndAddress());
    PageDirectory* directory = (PageDirectory*)_alloc.callocAligned(sizeof(PageDirectory), KIB4);
    _currentDirectory = directory;

    // reserve kernel physical memory
    phys_mem.lockPages((unsigned char*)0, (unsigned int)TOTAL_KERNEL_END_ADDR / PAGE_SIZE + 1);
    // reserve framebuffer physical memory
    phys_mem.lockPages((unsigned char*)screen.getFbStartAddress(), (unsigned int)screen.getFbLength() / PAGE_SIZE + 1);


    mapKernelMem();

    // enable();
    enable_paging(getPageDirectoryAddr());
    _currentPagingSys = this;

    _is_initialized = true;
}


Page* PagingSystem::getPage(uintptr_t address, bool make)
{
    address /= PAGE_SIZE; // get index from address (get the left most 22 bits)
    uint32_t table_idx = address / PAGE_COUNT; // get table index from address (get the left most 10 bits)

    if (_currentDirectory->pageTables[table_idx] != nullptr) // if pageTable is there return page
    {
        return &_currentDirectory->pageTables[table_idx]->pages[address % PAGE_COUNT]; // if page has been created return it (get the right most 10 bits)
    }
    else if (make)
    {
        //create pageTable and return page
        uintptr_t addr = (uintptr_t)kernelPaging.getAllocator()->mallocAligned(sizeof(PageTable), KIB4); // get a pageTable (aligned to 4KB
        if (addr == 0){
            return nullptr;
        }
        
        _currentDirectory->pageTables[table_idx] = (PageTable*)addr;
        memset(_currentDirectory->pageTables[table_idx], 0, PAGE_SIZE); // clean the memory
        
        _currentDirectory->pageDirectoryEntries[table_idx] = addr | 0x3; // setting the permissions PRESENT, RW, SV.
        return &_currentDirectory->pageTables[table_idx]->pages[address % PAGE_COUNT]; // return the page (get the right most 10 bits)
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
            screen.panic("No free frames!");
        }
    }
}


void PagingSystem::mapKernelMem()
{
    //identity paging for kernel
    identityPaging(0, (uintptr_t)(TOTAL_KERNEL_END_ADDR));
    //identity paging for framebuffer
    identityPaging(screen.getFbStartAddress(), screen.getFbStartAddress() + screen.getFbLength());
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

void PagingSystem::identityPaging(uintptr_t start, uintptr_t end)
{
    uintptr_t i = start;
    while (i < end)
    {
        Page* page = getPage(i, true);
        page->user_supervisor = 0;
        page->read_write = 1;
        page->present = 1;
        page->frame = (i >> 12);// set frame
        i += PAGE_SIZE;
    }
}

void PagingSystem::enable()
{
    switch_pd(getPageDirectoryAddr());
}

bool PagingSystem::pageFaultHandler(PageFaultError* pageFault, uintptr_t faultAddr)
{
    Page* page = nullptr;

    write_serial_var("page fault addr", faultAddr);

    //should kill process or send invalid addrs to it
    if (pageFault->protection_key)
    {
        write_serial("  protection key violation");
        return false;
    }

    // page is not presend and the fault occured by a write opperation
    if (!pageFault->present && pageFault->write != 0)
    {
        write_serial("  allocating memory for new page");
        page = getPage(faultAddr, true);

        page->present = 1;
        page->available = 1;
        page->accessed = 1;

        allocFrame(page, pageFault->user, pageFault->write);
        return true;
    }
    return false;
}

void PagingSystem::createPageDirectory()
{
    // FIXME: there is a problem here with the allocator, could be that something is not free but it always returns 0 for the address.
    // create page directory table in the kernel heap
    write_serial_var("kernel heap size", (uint32_t)(uintptr_t)kernelPaging.getAllocator()->malloc(100));
    PageDirectory* directory = (PageDirectory*)kernelPaging.getAllocator()->mallocAligned(sizeof(PageDirectory), 0x10);
    memset(directory, 0, sizeof(PageDirectory));
    write_serial_var("page directory addr", (uintptr_t)directory);
    _currentDirectory = directory;
}


PagingSystem* getCurrentPagingSys()
{
    return _currentPagingSys;
}


void PagingSystem::allocAddresses(uintptr_t start, uintptr_t end, bool user_supervisor, bool read_write)
{
    // check if start is aligned to 4KB
    if (start % PAGE_SIZE != 0)
    {
        start &= 0xFFFFF000;
        start += PAGE_SIZE;
        write_serial("*****start address not aligned to 4KB (in allocAddress function in virtual memory)*****");
    }

    for (uintptr_t i = start; i < end; i += PAGE_SIZE)
    {
        allocFrame(getPage(i, true), user_supervisor, read_write);
    }
}

void PagingSystem::freeAddresses(uintptr_t start, uintptr_t end)
{
    for (uintptr_t i = start; i < end; i += PAGE_SIZE)
    {
        freeFrame(getPage(i, false));
    }
}


uintptr_t PagingSystem::translateAddr(uintptr_t virtualAddr)
{
    uint32_t pd_index = (virtualAddr >> 22) & 0x3FF;
    uint32_t pt_index = (virtualAddr >> 12) & 0x3FF;

    if (!_currentDirectory->pageTables[pd_index]) {
        // Page table not present
        return 0; // Or handle appropriately
    }

    PageTable* pageTable = _currentDirectory->pageTables[pd_index];
    Page page = pageTable->pages[pt_index];

    if (!page.present) {
        // Page not present
        return 0; // Or handle appropriately
    }

    uintptr_t physicalAddress = (page.frame << 12) + (virtualAddr & 0xFFF);
    return physicalAddress;
}


uintptr_t PagingSystem::mapVirtToPhys(uintptr_t virtualAddr, uintptr_t physicalAddr)
{
    Page* page = getPage(virtualAddr, true);
    page->frame = physicalAddr >> 12;
    page->present = 1;
    page->user_supervisor = 0;
    page->read_write = 1;
    return virtualAddr;
}


uintptr_t* PagingSystem::getPageDirectoryAddr()
{
    return (uintptr_t*)_currentDirectory->pageDirectoryEntries;
}


Allocator* PagingSystem::getAllocator()
{
    return &_alloc;
}

PageDirectory* PagingSystem::getCurrentDirectory()
{
    return _currentDirectory;
}
