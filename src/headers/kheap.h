/*
basic heap for kernel for before paging heap is created, it is located in the first 4 MB of memory after the kernel end.
*/
#pragma once
#include "std.h"
#include "pageFrameAllocator.h"




// kernel allocate memory of size (in bytes) (from the first 4 MB of memory after the kernel end) 
// returns the virtual address of the memory
uintptr_t kmalloc(uint32_t size, bool align, uint32_t* phys);

// kernel malloc a size of memory (aligned)
// return the virtual address of the memory
uintptr_t kmallocAligned(uint32_t size);

// kernel malloc a size of mem and insert the physical address in phys  (not aligned)
// return the virtual address of the memory
uintptr_t kmallocPhys(uint32_t size, uint32_t* phys); 

// kernel malloc a size of mem and insert the physical address in phys  (aligned)
// return the virtual address of the memory
uintptr_t kmallocAlignedPhys(uint32_t size, uint32_t* phys);

void setPlacementAddr(uintptr_t addr);