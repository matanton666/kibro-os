#include "GDT.h"

GdtEntry gdtTable[3];
GdtPtr gdtPtr;
extern "C" void load_gdt(GdtEntry*);

// Set the value of a GDT entry.
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

void initGdt()
{
	//setting the gdt pointer
	gdtPtr.size = (sizeof(GdtEntry) * 3) - 1;
	gdtPtr.base = (uintptr_t)&gdtTable;

	//setting the null, code, data segments (Flat memory model)
	gdtSetGate(0, 0, 0, 0, 0);				// Null segment
	gdtSetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);	// Code segment
	gdtSetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);	// Data segment

	//loading the gdt to the cpu
	load_gdt((GdtEntry*) &gdtPtr);

	write_serial("GDT PTR at: ");
	write_serial_hex((uint64_t)&gdtPtr);
	write_serial("GDT at: ");
	write_serial_hex((uint64_t)gdtPtr.base);
}
