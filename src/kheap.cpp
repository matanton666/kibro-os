#include "headers/kheap.h"
// TODO: make case if kheap passes the of memory then panic or allocte more

uintptr_t placementAddr = 0; // initialized at pageFrameAllocator

uintptr_t kmalloc(uint32_t size, bool align, uint32_t* phys)
{
	if (align && (placementAddr & 0xFFFFF000)) // align address if its not page-alligned
	{
		placementAddr &= 0xFFFFF000;
		placementAddr += PAGE_SIZE;
	}
	if (phys)
	{
		*phys = placementAddr;
	}
	uintptr_t addr = placementAddr;
	placementAddr += size;
	return addr;
}

uintptr_t kmalloc_a(uint32_t size)
{
	return kmalloc(size, true, nullptr);
}

uintptr_t kmalloc_p(uint32_t size, uint32_t* phys)
{
	return kmalloc(size, false, phys);
}

uintptr_t kmalloc_ap(uint32_t size, uint32_t* phys)
{
	return kmalloc(size, true, phys);
}

void setPlacementAddr(uintptr_t addr)
{
	placementAddr = addr;
}
