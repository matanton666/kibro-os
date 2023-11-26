
#include "bitmap.h"


unsigned long bitmapBufferSize = 0;
unsigned char* bitmapBuffer = nullptr;


void bitmapSet(unsigned long index, bool value)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;
    
    // set bit to value
    bitmapBuffer[byteIndex] &= ~bit;
    if (value) {
        bitmapBuffer[byteIndex] |= bit;
    }
}

bool bitmapGet(unsigned long index)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;

    if ((bitmapBuffer[byteIndex] & bit) > 0) {
        return true;
    }
    else {
        return false;
    }
}