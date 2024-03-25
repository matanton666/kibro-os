// bitmap data structure
#pragma once

#include "std.h"
#include <stdint.h>


class BitMapDS
{
public:
    static bool getBit(uint8_t* bitmap, size_t bit);
    static void setBit(uint8_t* bitmap, size_t bit, bool value);
};