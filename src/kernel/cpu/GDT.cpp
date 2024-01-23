#include "../../headers/GDT.h"

GdtEntry gdtTable[GDT_SIZE];
GdtPtr gdtPtr;
extern "C" void load_gdt(GdtPtr*);


void gdtSetGate(unsigned short index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	//getting the address
	gdtTable[index].base_low = (base & 0xFFFF);
	gdtTable[index].base_middle = (base >> 16) & 0xFF;
	gdtTable[index].base_high = (base >> 24) & 0xFF;

	//getting the limit and granuality
	gdtTable[index].limit_low = (limit & 0xFFFF);
	gdtTable[index].granularity = (limit >> 16) & 0x0F;
	gdtTable[index].granularity |= gran & 0xF0;
	
	//getting the flags
	gdtTable[index].access = access;
}

void reloadGDT()
{
	load_gdt(&gdtPtr);
}

void initGdt()
{
	gdtPtr.size = (sizeof(GdtEntry) * GDT_SIZE) - 1;
	gdtPtr.base = (uintptr_t)&gdtTable;

	//setting the null, code, data segments (Flat memory model)
	gdtSetGate(0, 0, 0, 0, 0);				// Null segment
	gdtSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	// Code segment
	gdtSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	// Data segment
	gdtSetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);	// user code segment
	gdtSetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);	// user data segment
	gdtSetGate(5, 0, 0, 0, 0); // tss segment

	load_gdt(&gdtPtr);
}