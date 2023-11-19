#pragma once
#include <stdint.h>
#include "serial.h"
#include "std.h"

// get font file variables from symbol table
extern char _binary_res_Tamsyn8x16b_psf_start;
extern char _binary_res_Tamsyn8x16b_psf_end;

#define PSF1_FONT_MAGIC 0x0436
#define PSF2_FONT_MAGIC 0x864ab572

#define PSF1 1 
#define PSF2 2

#define PSF_START _binary_res_Tamsyn8x16b_psf_start
#define PSF_END _binary_res_Tamsyn8x16b_psf_end

#define CURSER_PADDING 10

// colors
#define COLOR_BLACK 0x00000000
#define COLOR_WHITE 0x00ffffff
#define COLOR_RED 0x00ff0000
#define COLOR_GREEN 0x0000ff00
#define COLOR_BLUE 0x000000ff


/*
width	how many pixels you have on a horizontal line
height	how many horizontal lines of pixels are present
pitch	how many bytes of VRAM you should skip to go one pixel down
depth	how many bits of color you have
bpp 	how many bytes of VRAM you should skip to go one pixel right.
*/

typedef struct  __attribute__((packed)) {
    uint32_t type; // should be 8
    uint32_t size; // should be around 32
    uint64_t addr; // address of the framebuffer
    uint32_t pitch; // how many bytes to skip to get to the next line
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
    uint8_t bpp; // how many bits per pixel
    uint8_t bufferType; // should be 1
    uint8_t reserved; // should be 0

}FramebufferInfo;

typedef struct __attribute__((packed)) {
    uint16_t magic; // for identification
    uint8_t fontMode;
    uint8_t characterSize;
} PSF1_Header;

typedef struct __attribute__((packed)) {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headerSize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numGlyph;      /* number of glyphs */
    uint32_t bytesPerGlyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF2_Header;

typedef struct __attribute__((packed)) {
    int x;
    int y;
} Point;

extern FramebufferInfo* fbInfo;

extern PSF1_Header* PSF1_font;
extern PSF2_Header* PSF2_font;

extern uint16_t* unicode;
extern Point curserPos;


bool initializeScreen();
void cls();
void print(char* str);
void print(char c);
void print(int num);



// get the framebuffer from the multiboot info
void initFramebuffer();

// draw a pixel on the screen
// x, y - position in pixels from the top left corner (0,0)
// color - 32 bit color (0xAARRGGBB)
void drawPixel( int x, int y, uint32_t color);

// check the version of the PSF font file
// return PSF1 or PSF2 or 0 if not found
unsigned int identifyPSFVersion();

// initialize the PSF font file and the unicode table
bool initPSF();

// draw char to screen with PSF2
// c - char to draw, cx, cy - position in pixels from the top left corner (0,0)
void putcPSF2( unsigned char c, int cx, int cy, uint32_t fgColor, uint32_t bgColor);

// draw string to screen with PSF2
// str - string to draw, startx, starty - position in pixels from the top left corner (0,0)
void putsPSF2( unsigned char* str, int startx, int starty, uint32_t fgColor, uint32_t bgColor);

// add point to curser position (x + curserPos.x, y + curserPos.y)
void curserAdd(int x, int y);

// set curser position to correct position on the screen if it is out of bounds
void curserCheckBounds();

// draw char to screen with PSF2 at the curser position
void putcCurserPSF2( unsigned char c, uint32_t fgColor, uint32_t bgColor);

// draw string to screen with PSF2 at the curser position
void putsCurserPSF2( unsigned char* str, uint32_t fgColor, uint32_t bgColor);

// draw entire screen in color
void clearScreen(uint32_t color);

