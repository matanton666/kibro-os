#pragma once
#include <stdint.h>
#include <stddef.h>


struct BootTag
{
    uint32_t type;
    uint32_t size;
}__attribute__((packed));
    


char* itoa(int num, char *str, int base);
void* memcpy(void *dest, const void *src, size_t n);
int memcmp(const void* lhs, const void* rhs, size_t count);


template <typename T>
T* getBootInfo(unsigned int tagType);

// template functions can only be implemented in the header file
template <typename T>
inline T* getBootInfo(unsigned int tagType)
{
	T* info = nullptr;
    // get boot info
    // move the boot info location to the pointer from ebx
    uint32_t* pBootInfo = 0;
    __asm("movl %%ebx, %0;" : "=r"(pBootInfo));

    // loop untill finds the framebuffer info
    uint32_t* addr = pBootInfo;
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
