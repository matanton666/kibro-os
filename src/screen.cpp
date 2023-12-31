#include "screen.h"

ScreenApi screen;

void cls()
{
    screen.clearScreen(COLOR_BLACK);
}

void print(const char* str)
{
    screen.putsCurserPSF2((unsigned char*)str, COLOR_WHITE, COLOR_BLACK);
}

void print(char c)
{
    screen.putcCurserPSF2(c, COLOR_WHITE, COLOR_BLACK);
}

void print(int num)
{
    char cnum[32];
    itoa(num, cnum, 10);
    screen.putsCurserPSF2((unsigned char*)cnum, COLOR_WHITE, COLOR_BLACK);
}

void print(uint32_t num)
{
    print((uint64_t)num);
}

void print(uint64_t num)
{
    char cnum[40];
    uitoa(num, cnum, 10);
    screen.putsCurserPSF2((unsigned char*)cnum, COLOR_WHITE, COLOR_BLACK);
}

void printBinary(uint64_t num)
{
    char cnum[64];
    uitoa(num, cnum, 2);
    screen.putsCurserPSF2((unsigned char*)cnum, COLOR_WHITE, COLOR_BLACK);
}

void printHex(uint64_t num)
{
    char cnum[32];
    uitoa(num, cnum, 16);
    screen.putsCurserPSF2((unsigned char*)cnum, COLOR_WHITE, COLOR_BLACK);
}

void panic(const char* str)
{
    screen.putsCurserPSF2((unsigned char *)"\nPANIC:\n", COLOR_RED, COLOR_BLUE);
    screen.putsCurserPSF2((unsigned char*)str, COLOR_RED, COLOR_BLACK);
    print('\n');
}



bool ScreenApi::init()
{
    return initializeScreen(getBootInfo<FramebufferInfo>(8));
}

void ScreenApi::drawPixel( int x, int y, uint32_t color)
{
    // get the pixel location 					              
    uint8_t* pixel = ((uint8_t*)_fbInfo->addr) + (_fbInfo->pitch * y) + (x * _fbInfo->bpp / 8); // divide by 8 because bpp is in bits and not bytes
    *pixel = color & 255; // blue color
    *(pixel + 1) = (color >> 8) & 255; // green color (shift 8 to get to it)
    *(pixel + 2) = (color >> 16) & 255; // red color (shift 16 to get to it)
}

unsigned int ScreenApi::identifyPSFVersion()
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

bool ScreenApi::initPSF()
{
    unsigned int ver = identifyPSFVersion();

    if (ver == PSF1)
    {
        //init for PSF vsrsion 1
        // PSF1_Header* font = (PSF1_Header*)&PSF_START;
    }
    else if (ver == PSF2)
    {
        _PSF2_font = (PSF2_Header*)&PSF_START;

        if (_PSF2_font->flags == 0) {
            _unicode = NULL;
            return false;
        }
        // initialize the unicode table
        // rest of unicode_table table translation once we have calloc() (memory managements)
        return true;
    }
    return false;
}

// ! does not move curser!! use with caution
void ScreenApi::putcPSF2( unsigned char c, int cx, int cy, uint32_t fgColor, uint32_t bgColor)
{
    int bytesPerLine = (_PSF2_font->width + 7) / 8;

    // if (unicode != NULL) { // translate unicode to ascii
    //     c = unicode[c];
    // }

    // check if the char is legal
    if (!(c > 0 && c < _PSF2_font->numGlyph)) {
        c = 0;
    }

    // get the chars glyph position in the font file
    unsigned char* glyph = (unsigned char*)&PSF_START + _PSF2_font->headerSize + c * _PSF2_font->bytesPerGlyph;

    // get offset to the top left corner of the current charicter
    uint32_t offs = (cy * _PSF2_font->height * _fbInfo->pitch) + (cx * (_PSF2_font->width + 1) * _fbInfo->bpp / 8);
    uint32_t x, y, line, mask;
    
    // print the char
    for (y = 0; y < _PSF2_font->height; y++) {
        line = offs;
        mask = 1 << (_PSF2_font->width - 1); // set mask to the left most bit (for deciding the color of the pixel)
        for (x = 0; x < _PSF2_font->width; x++) {
            drawPixel(x + cx, y + cy, *((unsigned int*)glyph) & mask ? fgColor : bgColor);
            // next pixel
            mask >>= 1;
            line += _fbInfo->bpp / 8;
        }
        // next row
        glyph += bytesPerLine;
        offs += _fbInfo->pitch;
    }
}


// ! does not move curser!! use with caution
void ScreenApi::putsPSF2(unsigned char* str, int startx, int starty, uint32_t fgColor, uint32_t bgColor)
{
    int i = 0;
    int x = startx, y = starty;

    while (str[i] != '\0')
    {
        putcPSF2(str[i], x, y, fgColor, bgColor);

        if (str[i] == '\n')
        {
            x = startx;
            y += _PSF2_font->height;
        }
        else {
            x += _PSF2_font->width;
        }
        i++;
    }
}

void ScreenApi::curserAdd(int x, int y)
{
    _curserPos.x += x;
    _curserPos.y += y;
    curserCheckBounds();
}

void ScreenApi::curserCheckBounds()
{
    // x after right of screen
    if (_curserPos.x >= _fbInfo->width - (CURSER_PADDING + _PSF2_font->width)) // padding
    {
        _curserPos.x = CURSER_PADDING;
        _curserPos.y += _PSF2_font->height;
    }
    // x before left of screen
    else if (_curserPos.x <= CURSER_PADDING)
    {
        _curserPos.x = CURSER_PADDING;
    }

    // y below bottom of screen
    if (_curserPos.y >= _fbInfo->height - (CURSER_PADDING + _PSF2_font->height)) // padding
    {
        cls(); // clear screen if goten to the bottom
        _curserPos.y = CURSER_PADDING;
    }
    // y above top of screen
    else if (_curserPos.y < CURSER_PADDING)
    {
        _curserPos.y = CURSER_PADDING;
    }
}

bool ScreenApi::initializeScreen(FramebufferInfo* fbInfo)
{
    if (_is_initialized) {
        return true;
    }
    _is_initialized = true;

    _fbInfo = fbInfo;

    if (_fbInfo == nullptr) {
        return false;
    }

    if (!initPSF()) {
        return false;
    }
    _fbLength = _fbInfo->pitch * _fbInfo->height;
    clearScreen(COLOR_BLACK);
    return true;
}

void ScreenApi::putcCurserPSF2( unsigned char c,uint32_t fgColor, uint32_t bgColor)
 {
    if (c == '\n')
    {
        curserAdd(-_curserPos.x + CURSER_PADDING, _PSF2_font->height);
    }
    else
    {
        int bytesPerLine = (_PSF2_font->width + 7) / 8;

        // if (unicode != NULL) { // translate unicode to ascii
        //     c = unicode[c];
        // }
        // check if the char is legal
        if (!(c > 0 && c < _PSF2_font->numGlyph)) {
            c = 0;
        }
        // get the chars glyph position in the font file
        unsigned char* glyph = (unsigned char*)&PSF_START + _PSF2_font->headerSize + c * _PSF2_font->bytesPerGlyph;

        // get offset to the top left corner of the current charicter
        uint32_t offs = (_curserPos.y * _PSF2_font->height * _fbInfo->pitch) + (_curserPos.x * (_PSF2_font->width + 1) * _fbInfo->bpp / 8);
        uint32_t x, y, line, mask;

        for (y = 0; y < _PSF2_font->height; y++) {
            line = offs;
            mask = 1 << (_PSF2_font->width - 1); // set mask to the left most bit (for deciding the color of the pixel)
            for (x = 0; x < _PSF2_font->width; x++) {
                curserCheckBounds();
                drawPixel(_curserPos.x + x, _curserPos.y + y, *((unsigned int*)glyph) & mask ? fgColor : bgColor);
                // next pixel
                mask >>= 1;
                line += _fbInfo->bpp / 8;
            }
            // next row
            glyph += bytesPerLine;
            offs += _fbInfo->pitch;
        }
        curserAdd(_PSF2_font->width, 0);
    }
 }

void ScreenApi::putsCurserPSF2( unsigned char* str, uint32_t fgColor, uint32_t bgColor)
 {
     int i = 0;
     while (str[i] != '\0')
     {
        putcCurserPSF2(str[i], fgColor, bgColor);
        i++;
     }
 }

void ScreenApi::clearScreen(uint32_t color)
{
    for (unsigned int y = 0; y < _fbInfo->height; y++)
    {
        for (unsigned int x = 0; x < _fbInfo->width; x++)
        {
            drawPixel(x, y, color);
        }
    }
    _curserPos.x = CURSER_PADDING;
    _curserPos.y = CURSER_PADDING;
}

void ScreenApi::setCursurPosition(int x, int y)
{
    _curserPos.x = x;
    _curserPos.y = y;
    curserCheckBounds();
}

const uint64_t ScreenApi::getFbStartAddress()
{
    return _fbInfo->addr;
}

const Point& ScreenApi::getCursur()
{
    return _curserPos;
}

unsigned long ScreenApi::getFbLength()
{
    return _fbLength;
}

