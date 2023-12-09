#include "GDT.h"

gdtEntry_t gdtTable[3];
gdtPtr_t gdtPtr;

void gdtSet(uint32_t gdtTable)
{
	__asm("cli"); // ignore interrupts
	__asm
	(
		"lgdt (%0)"   // Load the GDT using the lgdt instruction
		:
		: "r" (gdtTable)  // Input: Operand for the lgdt instruction
		: "memory"  // Clobber: Indicate that memory is being modified
	);
	__asm // reload code, data and other registries
	(
		"movw $0x10, %ax\n\t"   // Load the data segment value into AX
		"movw %ax, %ds\n\t"    // Move the value in AX to DS
		"movw %ax, %es\n\t"    // Move the value in AX to ES
		"movw %ax, %fs\n\t"    // Move the value in AX to FS
		"movw %ax, %gs\n\t"    // Move the value in AX to GS
		"movw %ax, %ss\n\t"    // Move the value in AX to SS
		"ret\n\t"             // Return
	);
	__asm("sti"); // allow intterupts
}

// Set the value of a GDT entry.
void gdtSetGate(unsigned short index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdtTable[index].base_low = (base & 0xFFFF);
	gdtTable[index].base_middle = (base >> 16) & 0xFF;
	gdtTable[index].base_high = (base >> 24) & 0xFF;

	gdtTable[index].limit_low = (limit & 0xFFFF);
	gdtTable[index].granularity = (limit >> 16) & 0x0F;

	gdtTable[index].granularity |= gran & 0xF0;
	gdtTable[index].access = access;
}

void initGdt()
{
	gdtPtr.size = (sizeof(gdtTable[0]) * 3) - 1;
	gdtPtr.base = (uint32_t)&gdtTable;

	gdtSetGate(0, 0, 0, 0, 0);				// Null segment
	gdtSetGate(1, 0, 0xFFFF, 0x9A, 0xCF);	// Code segment
	gdtSetGate(2, 0, 0xFFFF, 0x92, 0xCF);	// Data segment

	gdtSet((uint32_t)&gdtPtr);
}
