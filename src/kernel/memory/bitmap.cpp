#include "../../headers/bitmap.h"


void BitMap::set(unsigned long index, bool value)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;
    
    // set bit to value
    _buffer[byteIndex] &= ~bit;
    if (value) {
        _buffer[byteIndex] |= bit;
    }
}


bool BitMap::get(unsigned long index)
{
    unsigned long byteIndex = index / 8;
    unsigned char bitIndex = index % 8;
    unsigned char bit = 0b10000000 >> bitIndex;

    if ((_buffer[byteIndex] & bit) > 0) {
        return PAGE_LOCKED;
    }
    else {
        return PAGE_FREE;
    }
}

const unsigned long BitMap::getBufferSize()
{
    return _bufferSize;
}

const unsigned char* BitMap::getBufferStartAddress()
{
    return _buffer;
}

MemoryMapApi* BitMap::getMemMapApi()
{
    return &_mem_map_api;
}

bool BitMap::init()
{
    if (_is_initialized) {
        return true;
    }
    _is_initialized = true;

    _mem_map_api.init();

    // 4kib memory per page and one bit representing each page, plus one just in case
    _bufferSize = (_mem_map_api.getFreeMem() / PAGE_SIZE / 8) + 1; // in bytes
    _buffer = (unsigned char*)_mem_map_api.getLargestFreeSegment();// begins at the start of the largest memroy segment

    // make sure bitmap is after kernel code and not overwriting it
    if (END_OF_KERNEL_CODE > (uintptr_t)_buffer) {
        _buffer = (unsigned char*)(uint64_t)END_OF_KERNEL_CODE;
    }

    memset(_buffer, 0, _bufferSize);
    return true;
}
