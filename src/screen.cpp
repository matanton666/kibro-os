#include "headers/screen.h"
#include "screen.h"

FramebufferInfo* initFramebuffer() {
    // get boot info
    // move the boot info location to the pointer from ebx
	unsigned long* bootInfo = 0;
	__asm("movl %%ebx, %0;" : "=r"(bootInfo));
	// loop untill finds the framebuffer info
	long unsigned i = 0;
	for (; i < bootInfo[0] / 4 && (bootInfo[i] != 8 || bootInfo[i+1] != 38); i++) {}
	// instert into the framebufferInfo struct the info from the bootInfo
	return (struct FramebufferInfo*)(bootInfo + i);
}

void drawPixel(FramebufferInfo* fbInfo, int x, int y, int color) // color is represented by 0x00RRGGBB
{
	// get the pixel location 					              
    uint8_t* pixel = ((uint8_t*)fbInfo->addr) + (fbInfo->pitch * y) + (x * fbInfo->bpp / 8); // devide by 8 because bpp is in bits and not bytes
    *pixel = color & 255; // blue color
    *(pixel +1) = (color >> 8) & 255; // green color (shift 8 to get to it)
    *(pixel +2) = (color >> 16) & 255; // red color (shift 16 to get to it)
}
