/*
a map of all the usable memory in the system
for tracking which pages are in use and which are free
*/
#pragma once

#include "std.h"


extern unsigned long bitmapBufferSize;
extern unsigned char* bitmapBuffer;

void bitmapSet(unsigned long index, bool value);
bool bitmapGet(unsigned long index);