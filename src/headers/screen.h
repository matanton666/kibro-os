/*
manage the output screen of the os
*/
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

#define CURSER_PADDING 16



enum COLORS
{
    BLACK = 0x00000000,
    WHITE = 0x00ffffff,
    RED = 0x00ff0000,
    GREEN = 0x0000ff00,
    BLUE = 0x000000ff,
    YELLOW = 0x00ffff00,
    CYAN = 0x0000ffff,
    GREY = 0x00a9a9a9,
    ORANGE = 0x00ffa500,
    PURPLE = 0x00800080,
    BROWN = 0x00a52a2a,
    PINK = 0x00ffc0cb,
};


/*
width	how many pixels you have on a horizontal line
height	how many horizontal lines of pixels are present
pitch	how many bytes of VRAM you should skip to go one pixel down
depth	how many bits of color you have
bpp 	how many bytes of VRAM you should skip to go one pixel right.
*/

struct FramebufferInfo {
    uint32_t type; // should be 8
    uint32_t size; // should be around 32
    uint64_t addr; // address of the framebuffer
    uint32_t pitch; // how many bytes to skip to get to the next line
    uint32_t width; // width in pixels
    uint32_t height; // height in pixels
    uint8_t bpp; // how many bits per pixel
    uint8_t bufferType; // should be 1
    uint8_t reserved; // should be 0

}__attribute__((packed));

struct  PSF1_Header {
    uint16_t magic; // for identification
    uint8_t fontMode;
    uint8_t characterSize;
} __attribute__((packed)) ;

struct PSF2_Header {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headerSize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numGlyph;      /* number of glyphs */
    uint32_t bytesPerGlyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
}__attribute__((packed));

struct Point {
    unsigned int x;
    unsigned int y;
}__attribute__((packed));


class ScreenApi
{
private:
    FramebufferInfo* _fbInfo = nullptr;
    PSF1_Header* _PSF1_font = nullptr;
    PSF2_Header* _PSF2_font = nullptr;

    uint16_t* _unicode = nullptr;
    Point _curserPos = { CURSER_PADDING, CURSER_PADDING };
    unsigned long _fbLength = 0;
    bool _is_initialized = false;
    bool _cursorShow = false;
    COLORS _textColor = WHITE;
    COLORS _bgColor = BLACK;
    bool _is_printing = false;
    unsigned int _curosr_width = 7;



    // draw a pixel on the screen
    // x, y - position in pixels from the top left corner (0,0)
    // color - 32 bit color (0xAARRGGBB)
    void drawPixel( int x, int y, uint32_t color);

    // clear the last amount of pixel (need to define size of row)
    void clearLastPixels(int rows, int amount);

    // check the version of the PSF font file
    // return PSF1 or PSF2 or 0 if not found
    unsigned int identifyPSFVersion();

    // initialize the PSF font file and the unicode table
    bool initPSF();

    // add point to curser position (x + curserPos.x, y + curserPos.y)
    void curserAdd(int x, int y);

    // set curser position to correct position on the screen if it is out of bounds
    void curserCheckBounds();

    bool initializeScreen(FramebufferInfo* fbInfo);

public:
    bool init();

    void cls();
    void println(const char* str);
    void print(const char* str);
    void print(char c);
    void print(int num);
    void print(float num);
    void print(uint32_t num);
    void print(uint64_t num);
    void printBinary(uint64_t num);
    void printHex(uint64_t num);
    void newLine();
    void panic(const char* str);
    void clearLastChar();

    // draw char to screen with PSF2 at the curser position
    void putcCurserPSF2( unsigned char c, uint32_t fgColor, uint32_t bgColor);

    // draw string to screen with PSF2 at the curser position
    void putsCurserPSF2( unsigned char* str, uint32_t fgColor, uint32_t bgColor);

    // draw char to screen with PSF2 (without moving the curser)
    // c - char to draw, cx, cy - position in pixels from the top left corner (0,0)
    void putcPSF2( unsigned char c, int cx, int cy, uint32_t fgColor, uint32_t bgColor);

    // draw string to screen with PSF2 (without moving the curser)
    // str - string to draw, startx, starty - position in pixels from the top left corner (0,0)
    void putsPSF2( unsigned char* str, int startx, int starty, uint32_t fgColor, uint32_t bgColor);
    
    // draw entire screen in color
    void clearScreen(uint32_t color);

    void clearLastChars(int n);

    // set curser position (will set to bounds if out of bounds)
    void setCursurPosition(int x, int y);

    // show and clear the cursor
    void showCursor();
    void clearCursor();

    // get the starting address of the framebuffer (where the pixels start)
    const uint32_t getFbStartAddress();

    // get the x and y cordiantes of the curser
    const Point& getCursur();

    bool isCursorShow();

    // get the length of the framebuffer in bytes
    unsigned long getFbLength();

    // set the text color
    void setTextColor(COLORS color);

    // set the background color
    void setBgColor(COLORS color);

    // function for the PIT to check if the cursor should blink
    bool isPrinting();

    void setCursorWidth(unsigned int width);

    unsigned int getCursorWidth();
};


extern ScreenApi screen;


