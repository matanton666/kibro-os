#pragma once

#include <stdint.h>

/*
width	how many pixels you have on a horizontal line
height	how many horizontal lines of pixels are present
pitch	how many bytes of VRAM you should skip to go one pixel down
depth	how many bits of color you have
bpp 	how many bytes of VRAM you should skip to go one pixel right.
*/

struct FramebufferInfo {
	uint32_t type;
	uint32_t size;
	uint64_t addr;
	uint32_t pitch;
	uint32_t width;
	uint32_t height;
	uint8_t bpp;
	uint8_t bufferType;
	uint8_t reserved;

}__attribute__((__packed__));


FramebufferInfo* initFramebuffer();
void drawPixel(FramebufferInfo* fbInfo, int x, int y, int color);