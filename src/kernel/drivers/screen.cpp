#include "../../headers/screen.h"

ScreenApi screen;

void ScreenApi::cls()
{
    screen.clearScreen(_bgColor);
}

void ScreenApi::println(const char* str)
{
    screen.putsCurserPSF2((unsigned char*)str, _textColor, _bgColor);
    print('\n');
}

void ScreenApi::print(const char* str)
{
    screen.putsCurserPSF2((unsigned char*)str, _textColor, _bgColor);
}

void ScreenApi::print(char c)
{
    screen.putcCurserPSF2(c, _textColor, _bgColor);
}

void ScreenApi::print(int num)
{
    char cnum[32];
    itoa(num, cnum, 10);
    screen.putsCurserPSF2((unsigned char*)cnum, _textColor, _bgColor);
}

void ScreenApi::print(float num) 
{
    char cnum[32];
    memset(cnum, 0, 32);
    ftoa(num, cnum, 10);
    screen.putsCurserPSF2((unsigned char*)cnum, _textColor, _bgColor);
}

void ScreenApi::print(uint32_t num)
{
    print((uint64_t)num);
}

void ScreenApi::print(uint64_t num)
{
    char cnum[40];
    uitoa(num, cnum, 10);
    screen.putsCurserPSF2((unsigned char*)cnum, _textColor, _bgColor);
}

void ScreenApi::printBinary(uint64_t num)
{
    char cnum[64];
    uitoa(num, cnum, 2);
    screen.putsCurserPSF2((unsigned char*)cnum, _textColor, _bgColor);
}

void ScreenApi::printHex(uint64_t num)
{
    char cnum[32];
    uitoa(num, cnum, 16);
    screen.putsCurserPSF2((unsigned char*)cnum, _textColor, _bgColor);
}

void ScreenApi::newLine()
{
    print('\n');
}

void ScreenApi::panic(const char* str)
{
    screen.putsCurserPSF2((unsigned char *)"\nPANIC:\n", COLORS::RED, COLORS::CYAN);
    screen.putsCurserPSF2((unsigned char*)str, COLORS::RED, _bgColor);
    print('\n');
}

void ScreenApi::clearLastChar()
{
    screen.clearCursor();
    screen.clearLastChars(1);
    screen.showCursor();
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

void ScreenApi::clearLastPixels(int rows, int amount) 
{
    _is_printing = true;
    int x = getCursur().x - 1;
    int y = getCursur().y;

    for(int i = 0; i < rows; i++)
    {
        if (x <= CURSER_PADDING) // got to the left of the screen
        {
            write_serial("got to end");
            y -= amount;
            x = _fbInfo->width - CURSER_PADDING -1;
            _curserPos.x = x;
            _curserPos.y = y;
            return;
        }
        for (int j = 0; j < amount; j++)
        {
            drawPixel(x, y, _bgColor);
            y++;
        }
        

        y = getCursur().y;
        x--;
    }
    curserAdd(-1 * rows,0);
    _is_printing = false;
}

void ScreenApi::clearLastChars(int n)
{
    for(int i = 0; i < n; i++)
    {
        clearLastPixels(_PSF2_font->width, _PSF2_font->height);
    }
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

    _fbInfo = fbInfo;

    write_serial_var("fb addr", _fbInfo->addr);

    if (_fbInfo == nullptr) {
        return false;
    }

    if (!initPSF()) {
        return false;
    }
    _fbLength = _fbInfo->pitch * _fbInfo->height;
    clearScreen(_bgColor);
    
    _is_initialized = true;
    return true;
}

void ScreenApi::putcCurserPSF2( unsigned char c,uint32_t fgColor, uint32_t bgColor)
 {
    _is_printing = true;

    if (isCursorShow())
        clearCursor();

    if (c == '\n')
    {
        curserAdd(-_curserPos.x + CURSER_PADDING, _PSF2_font->height);
    }
    else if (c == '\t') 
    {
        curserAdd(4 * _PSF2_font->width, 0);
    }
    else if (c == '\b') // backspace
    {
        clearLastChars(1);
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
    showCursor();
    _is_printing = false;
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
    _is_printing = true;
    for (unsigned int y = 0; y < _fbInfo->height; y++)
    {
        for (unsigned int x = 0; x < _fbInfo->width; x++)
        {
            drawPixel(x, y, color);
        }
    }
    _is_printing = false;
    _curserPos.x = CURSER_PADDING;
    _curserPos.y = CURSER_PADDING;
}

void ScreenApi::setCursurPosition(int x, int y)
{
    _curserPos.x = x;
    _curserPos.y = y;
    curserCheckBounds();
}

bool ScreenApi::isCursorShow()
{
    return _cursorShow;
}

void ScreenApi::showCursor()
{
    if (_cursorShow) return;
    _is_printing = true;

    int x = _curserPos.x + 1;
    int y = _curserPos.y - 1;
    for (int i = 0; i < _curosr_width; i++)
    {
        for (int j = 0; j < _PSF2_font->height + 2; j++)
        {
            drawPixel(x, y, _textColor);
            y++;
        }

        y = _curserPos.y - 1;
        x++;
    }
    _cursorShow = true;
    _is_printing = false;
}
void ScreenApi::clearCursor()
{
    if (!_cursorShow) return;

    _is_printing = true;
    int x = _curserPos.x + 1;
    int y = _curserPos.y - 2;
    for (int i = 0; i < _curosr_width; i++)
    {
        for (int j = 0; j < _PSF2_font->height + 4; j++)
        {
            drawPixel(x, y, _bgColor);
            y++;
        }

        y = _curserPos.y - 2;
        x++;
    }
    _cursorShow = false;
    _is_printing = false;
}

const uint32_t ScreenApi::getFbStartAddress()
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

void ScreenApi::setTextColor(COLORS color)
{
    _textColor = color;
}

void ScreenApi::setBgColor(COLORS color)
{
    _bgColor = color;
}

bool ScreenApi::isPrinting()
{
    return _is_printing;
}


void ScreenApi::setCursorWidth(unsigned int width)
{
    if (width >= 1 && width <= 8)
    {
        clearCursor();
        _curosr_width = width;
        showCursor();
    }
}


unsigned int ScreenApi::getCursorWidth()
{
    return _curosr_width;
}

