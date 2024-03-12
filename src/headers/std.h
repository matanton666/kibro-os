/*
our implementation of the standard library
*/
#pragma once
#include <stdint.h>
#include <stddef.h>

#define KIB1 1024 // in bytes
#define KIB4 4096 // in bytes
#define MIB1 1048576 // in bytes
#define GIB1 1073741824 // in bytes

struct BootTag
{
    uint32_t type;
    uint32_t size;
}__attribute__((packed));

extern uint32_t* bootInfoPtr; //! DO NOT TOUCH THIS VARIABLE!!!


// ** string and number functions **

// int to ascii string
char* itoa(int num, char *str, int base);

// unsigned int to ascii string
char* uitoa(uint64_t num, char *str, int base);

// floating point number to ascii string
char* ftoa(float num, char* str, int base);


unsigned int strlen(const char* str);

// concatenate two strings
char* strcat(char* dest, const char* src);

// compare two strings (ret=0 if equal, ret>0 if str1 > str2, ret<0 if str1 < str2)
int strcmp(const char* str1, const char* str2);

// ** memory functions **

// copy n bytes from src to dest
void* memcpy(void *dest, const void *src, size_t n);

// compare n bytes from lhs to rhs
int memcmp(const void* lhs, const void* rhs, size_t count);

// set n bytes of dest to ch
void memset(void* dest, int ch, uint32_t count);

// a quicker implementation of memset for setting memory to zero
void zeroMemory(void* dest, uint32_t count);

uintptr_t align_up(uintptr_t address, uintptr_t alignment);


// ** serial port communication **

// write to serial output port
void outb(uint16_t port, uint8_t value);

// get input from serial port
uint8_t inb(uint16_t port);

// write 32 bit to bus
void outl(uint16_t port, uint32_t value);

// read 32 bit from bus
uint32_t inl(uint16_t port);

// wait one io cycle for devices to catch up on input/output
void ioWait();


// ** assembly functions **

// asm cli instruction
void cli();

// asm sti instruction
void sti();



// ** bootloader communication **

// store the address of the boot info struct from grub
// this functions should be called at the beggining of the kernel so not to loose the address
void getBootInfoAddressFromGrub();

// get a boot info tag from grub using the tag type
template <typename T>
T* getBootInfo(unsigned int tagType);

// template functions can only be implemented in the header file
template <typename T>
inline T* getBootInfo(unsigned int tagType)
{
	T* info = nullptr;
    // move the boot info location to the pointer from ebx
    getBootInfoAddressFromGrub();

    // loop untill finds the framebuffer info
    uint32_t* addr = bootInfoPtr;
    for (BootTag* tag = (BootTag *)(&addr[2]);
       tag->type != 0;
       tag = (BootTag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) // move to next tag (with padding)
    {
		if (tag->type == tagType) {
			info = (T*)tag;
			break;
		}
	}

	return info;
}