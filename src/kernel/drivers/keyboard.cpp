#include "../../headers/keyboard.h"

Keyboard keyboard;

void Keyboard::handleKeyboard(uint8_t scancode) {

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
    case BACKSPACE:
        screen.clearLastChar();
        return;
    case ENTER:
        screen.newLine();
        return;
    case SPACE_BAR:
        screen.print(' ');
        return;
    case TAB:
        screen.print('\t');
        return;
    }

    char ch = translateScanCode(scancode, _rightShift || _leftShift, _capsLock);
    if (ch != 0) {
        screen.print(ch);
    }

}

char Keyboard::translateScanCode(uint8_t scancode, bool shift, bool capsLock)
{
    // TODO: add support for more keys (ex. '{', '?', ...)
    if (scancode > 58) 
        return 0;
  
    int ch = (int)ASCIITable[scancode];

    if (shift)
        return isAlpha(ch) ? toUpper(ch) : shiftDigit(ch);
    else if (capsLock)
        return toUpper(ch);

    return ch;
}

void keyboardHandler(uint8_t scancode)
{
    keyboard.handleKeyboard(scancode);
}
