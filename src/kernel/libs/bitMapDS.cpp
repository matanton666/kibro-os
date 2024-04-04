#include "../../headers/bitMapDS.h"

bool BitMapDS::getBit(uint8_t* bitmap, size_t bit)
{
    return bitmap[bit / 8] & (1 << (bit % 8));
}

void BitMapDS::setBit(uint8_t* bitmap, size_t bit, bool value)
{
    if (value)
        bitmap[bit / 8] |= (1 << (bit % 8));
    else
        bitmap[bit / 8] &= ~(1 << (bit % 8));
}