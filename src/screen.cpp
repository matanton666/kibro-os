#include "screen.h"

FramebufferInfo* fbInfo = nullptr;

PSF1_Header* PSF1_font = nullptr;
PSF2_Header* PSF2_font = nullptr;

uint16_t* unicode = nullptr;
Point curserPos = { CURSER_PADDING, CURSER_PADDING };

bool initializeScreen()
{
    initFramebuffer();
    if (fbInfo == nullptr) {
        return false;
    }

    if (!initPSF()) {
        return false;
    }

    clearScreen(COLOR_BLACK);
    return true;
}

void cls()
{
    clearScreen(COLOR_BLACK);
}

void print(char* str)
{
    putsCurserPSF2((unsigned char*)str, COLOR_WHITE, COLOR_BLACK);
}

void print(char c)
{
    putcCurserPSF2(c, COLOR_WHITE, COLOR_BLACK);
}

void print(int num)
{
    char cnum[32];
    itoa(num, cnum, 10);
    putsCurserPSF2((unsigned char*)cnum, COLOR_WHITE, COLOR_BLACK);
}

void initFramebuffer()
{
    struct BootTag
    {
        uint32_t type;
        uint32_t size;
    };
    
    // get boot info
    // move the boot info location to the pointer from ebx
    uint32_t* bootInfo = 0;
    __asm("movl %%ebx, %0;" : "=r"(bootInfo));

    // loop untill finds the framebuffer info
    uint32_t* addr = bootInfo;
    for (BootTag* tag = (BootTag *)(&addr[2]);
       tag->type != 0;
       tag = (BootTag *) ((uint8_t *) tag + ((tag->size + 7) & ~7))) // move to next tag (with padding)
    {
        switch (tag->type)
        {
        case 8: // framebuffer tag
            fbInfo = (FramebufferInfo*)tag;
            break;

        default:
            break;
        }
    }
}

void drawPixel( int x, int y, uint32_t color)
{
    // get the pixel location 					              
    uint8_t* pixel = ((uint8_t*)fbInfo->addr) + (fbInfo->pitch * y) + (x * fbInfo->bpp / 8); // divide by 8 because bpp is in bits and not bytes
    *pixel = color & 255; // blue color
    *(pixel + 1) = (color >> 8) & 255; // green color (shift 8 to get to it)
    *(pixel + 2) = (color >> 16) & 255; // red color (shift 16 to get to it)
}

unsigned int identifyPSFVersion()
{
    PSF1_Header* font1 = (PSF1_Header*)&PSF_START;
    PSF2_Header* font2 = (PSF2_Header*)&PSF_START;

    if (font1->magic == PSF1_FONT_MAGIC) {
        return PSF1;
    }
    if (font2->magic == PSF2_FONT_MAGIC) {
        return PSF2;
    }

    return 0;
}

bool initPSF()
{
    unsigned int ver = identifyPSFVersion();

    if (ver == PSF1)
    {
        //init for PSF vsrsion 1
        // PSF1_Header* font = (PSF1_Header*)&PSF_START;
    }
    else if (ver == PSF2)
    {
        PSF2_font = (PSF2_Header*)&PSF_START;

        if (PSF2_font->flags == 0) {
            unicode = NULL;
            return false;
        }
        // initialize the unicode table
        // rest of unicode_table table translation once we have calloc() (memory managements)
        return true;
    }
    return false;
}

void putcPSF2( unsigned char c, int cx, int cy, uint32_t fgColor, uint32_t bgColor)
{
    int bytesPerLine = (PSF2_font->width + 7) / 8;

    // if (unicode != NULL) { // translate unicode to ascii
    //     c = unicode[c];
    // }

    // check if the char is legal
    if (!(c > 0 && c < PSF2_font->numGlyph)) {
        c = 0;
    }

    // get the chars glyph position in the font file
    unsigned char* glyph = (unsigned char*)&PSF_START + PSF2_font->headerSize + c * PSF2_font->bytesPerGlyph;

    // get offset to the top left corner of the current charicter
    int offs = (cy * PSF2_font->height * fbInfo->pitch) + (cx * (PSF2_font->width + 1) * fbInfo->bpp / 8);
    int x, y, line, mask;
    
    // print the char
    for (y = 0; y < PSF2_font->height; y++) {
        line = offs;
        mask = 1 << (PSF2_font->width - 1); // set mask to the left most bit (for deciding the color of the pixel)
        for (x = 0; x < PSF2_font->width; x++) {
            drawPixel(x + cx, y + cy, *((unsigned int*)glyph) & mask ? fgColor : bgColor);
            // next pixel
            mask >>= 1;
            line += fbInfo->bpp / 8;
        }
        // next row
        glyph += bytesPerLine;
        offs += fbInfo->pitch;
    }
}

void curserAdd(int x, int y)
{
    curserPos.x += x;
    curserPos.y += y;
    curserCheckBounds();
}

void curserCheckBounds()
{
    if (curserPos.x >= fbInfo->width - (CURSER_PADDING + PSF2_font->width)) // padding
    {
        curserPos.x = CURSER_PADDING;
        curserPos.y += PSF2_font->height;
    }
    else if (curserPos.x <= CURSER_PADDING)
    {
        curserPos.x = CURSER_PADDING;
    }

    if (curserPos.y >= fbInfo->height - (CURSER_PADDING + PSF2_font->height)) // padding
    {
        curserPos.y = CURSER_PADDING;
    }
    else if (curserPos.y <= CURSER_PADDING)
    {
        // TODO: clear screen or something
        curserPos.y = CURSER_PADDING;
    }
}

void putcCurserPSF2( unsigned char c,uint32_t fgColor, uint32_t bgColor)
 {
    if (c == '\n')
    {
        curserAdd(-curserPos.x + CURSER_PADDING, PSF2_font->height);
    }
    else
    {
        int bytesPerLine = (PSF2_font->width + 7) / 8;

        // if (unicode != NULL) { // translate unicode to ascii
        //     c = unicode[c];
        // }
        // check if the char is legal
        if (!(c > 0 && c < PSF2_font->numGlyph)) {
            c = 0;
        }
        // get the chars glyph position in the font file
        unsigned char* glyph = (unsigned char*)&PSF_START + PSF2_font->headerSize + c * PSF2_font->bytesPerGlyph;

        // get offset to the top left corner of the current charicter
        int offs = (curserPos.y * PSF2_font->height * fbInfo->pitch) + (curserPos.x * (PSF2_font->width + 1) * fbInfo->bpp / 8);
        int x, y, line, mask;

        for (y = 0; y < PSF2_font->height; y++) {
            line = offs;
            mask = 1 << (PSF2_font->width - 1); // set mask to the left most bit (for deciding the color of the pixel)
            for (x = 0; x < PSF2_font->width; x++) {
                curserCheckBounds();
                drawPixel(curserPos.x + x, curserPos.y + y, *((unsigned int*)glyph) & mask ? fgColor : bgColor);
                // next pixel
                mask >>= 1;
                line += fbInfo->bpp / 8;
            }
            // next row
            glyph += bytesPerLine;
            offs += fbInfo->pitch;
        }
        curserAdd(PSF2_font->width, 0);
    }
 }

void putsPSF2( unsigned char* str, int startx, int starty, uint32_t fgColor, uint32_t bgColor)
{
    int i = 0;
    int x = startx, y = starty;

    while (str[i] != '\0')
    {
        putcPSF2(str[i], x, y, fgColor, bgColor);

        if (str[i] == '\n')
        {
            x = startx;
            y += PSF2_font->height;
        }
        else {
            x += PSF2_font->width;
        }
        i++;
    }
}

 void putsCurserPSF2( unsigned char* str, uint32_t fgColor, uint32_t bgColor)
 {
     int i = 0;
     while (str[i] != '\0')
     {
         putcCurserPSF2(str[i], fgColor, bgColor);
         i++;
     }
 }

void clearScreen(uint32_t color)
{
    for (int y = 0; y < fbInfo->height; y++)
    {
        for (int x = 0; x < fbInfo->width; x++)
        {
            drawPixel(x, y, color);
        }
    }
    curserPos.x = CURSER_PADDING;
    curserPos.y = CURSER_PADDING;
}