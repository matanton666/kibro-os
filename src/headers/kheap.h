#pragma once
#include "std.h"
#include "pageFrameAllocator.h"

//basic heap of kernel cuurently using a placement address

extern uintptr_t placementAddr;

//allocate memory
uintptr_t kmalloc(uint32_t size, bool align, uint32_t* phys);
uintptr_t kmalloc_a(uint32_t size);  // page aligned.
uintptr_t kmalloc_p(uint32_t size, uint32_t* phys); // returns a physical address.
uintptr_t kmalloc_ap(uint32_t size, uint32_t* phys); // page aligned and returns a physical address.
void setPlacementAddr(uintptr_t addr);