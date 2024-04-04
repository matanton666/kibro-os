#include "../../headers/keyboard.h"

Keyboard keyboard;

void Keyboard::checkSpecialChar(uint8_t scancode) {

    // check if scancode has special key
    switch (scancode) {
    case L_SHIFT:
        _leftShift = true;
        return;
    case L_SHIFT + 0x80:
        _leftShift = false;
        return;
    case R_SHIFT:
        _rightShift = true;
        return;
    case R_SHIFT + 0x80:
        _rightShift = false;
        return;
    case CTRL:
        ctrl = true;
        return;
    case CTRL + 0x80:
        ctrl = false;
        return;
    case ALT:
        alt = true;
        return;
    case ALT + 0x80:
        alt = false;
        return;
    case CAPS_LOCK:
        _capsLock = !_capsLock;
        return;
    }
}

char Keyboard::translateScanCode(uint8_t scancode, bool shift, bool capsLock)
{
    if (scancode > 58) 
    {
        return 0;
    }
  
    int ch = (int)ASCIITable[scancode];

    if (shift)
        return isAlpha(ch) ? toUpper(ch) : shiftNonAlpha(ch);
    else if (capsLock)
        return toUpper(ch);

    return ch;
}


char Keyboard::getChar(uint8_t scancode)
{
    checkSpecialChar(scancode);
    return translateScanCode(scancode, _rightShift || _leftShift, _capsLock);
}

void Keyboard::printScancode(uint8_t scancode)
{
    char ch = keyboard.getChar(scancode);
    if (ch != 0) {
        screen.print(ch);
    }
}


void Keyboard::sendToQueue(uint8_t scancode)
{
    char ch = keyboard.getChar(scancode);
    if (ch != 0) {
        _buffer[_writeIndex] = ch;
        _writeIndex++;
        
        if (_writeIndex >= KEYBOARD_BUFFER_SIZE-1) {
            _writeIndex = 0;
        }
    }
}


char Keyboard::getFromQueue()
{
    if (_readIndex == _writeIndex) {
        return 0;
    }

    char ch = _buffer[_readIndex];
    _readIndex++;
    if (_readIndex >= KEYBOARD_BUFFER_SIZE-1) {
        _readIndex = 0;
    }
    return ch;
}


bool Keyboard::hasBeenPressed()
{
    return _writeIndex != _readIndex;
}



void Keyboard::reset()
{
    _leftShift = false;
    _rightShift = false;
    _capsLock = false;
    alt = false;
    ctrl = false;
    _writeIndex = 0;
    _readIndex = 0;
}
