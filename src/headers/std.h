/*
our implementation of the standard library
*/
#pragma once
#include <stdint.h>
#include <stddef.h>

struct BootTag
{
    uint32_t type;
    uint32_t size;
}__attribute__((packed));

extern uint32_t* bootInfoPtr; //! DO NOT TOUCH THIS VARIABLE!!!

    

// int to ascii string
char* itoa(int num, char *str, int base);

// unsigned int to ascii string
char* uitoa(uint64_t num, char *str, int base);

// copy n bytes from src to dest
void* memcpy(void *dest, const void *src, size_t n);

// compare n bytes from lhs to rhs
int memcmp(const void* lhs, const void* rhs, size_t count);

// write to serial output port
void outb(unsigned short port, unsigned char value);



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
